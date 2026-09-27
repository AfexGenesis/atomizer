#include "core/molecule/topology.hpp"
#include "core/molecule/linkage.hpp"
#include "core/molecule/template.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <fstream>
#include <unordered_map>
#include <DirectXMath.h>

namespace {
struct connection {
    std::string firstchain;
    std::string firstcomp;
    std::string firstatom;
    int firstseq = 0;
    std::string secondchain;
    std::string secondcomp;
    std::string secondatom;
    int secondseq = 0;
    bondorder order = bondorder::single;
};

struct residue {
    std::string chain;
    std::string comp;
    int seq = 0;
    std::unordered_map<std::string, uint32_t> atoms;
};

std::vector<std::string> split(const std::string &line){
    std::vector<std::string> values;
    for (size_t i = 0; i < line.size();){
        if (std::isspace(static_cast<unsigned char>(line[i]))){
            ++i;
            continue;
        }
        
        std::string value;
        if (line[i] == '\'' || line[i] == '"'){
            const char quote = line[i++];
            while (i < line.size() && line[i] != quote) value += line[i++];
            if (i < line.size()) ++i;
        }else{
            while (i < line.size() && !std::isspace(static_cast<unsigned char>(line[i]))) value += line[i++];
        }
        values.push_back(std::move(value));
    }
    return values;
}

std::string field(const std::unordered_map<std::string, size_t> &columns,
    const std::vector<std::string> &row, const std::string &name){
    const auto found = columns.find(name);

    return found != columns.end() && found->second < row.size() ? row[found->second] : "";
}

int number(const std::string &value, int fallback = 0){
    if (value.empty() || value == "." || value == "?") return fallback;
    try {
        return std::stoi(value);
        }catch (...){
    return fallback;
    }
}

uint64_t bondkey(uint32_t first, uint32_t second){
    if (first > second) std::swap(first, second);
    return (static_cast<uint64_t>(first) << 32) | second;
}

bool addbond(topology &result, std::unordered_map<uint64_t, size_t> &known, uint32_t first, uint32_t second, bondorder order){
    if (first == second) return false;
    if (first > second) std::swap(first, second);
    const uint64_t key = bondkey(first, second);
    const auto found = known.find(key);

    if (found != known.end()){
        bond &current = result.bonds[found->second];
        if (bondrank(order) > bondrank(current.order)) current.order = order;
        return false;
    }
    known.emplace(key, result.bonds.size());
    result.bonds.push_back({first, second, order});
    return true;
}

std::string residuekey(const std::string &chain, int seq, const std::string &comp){
    return chain + "|" + std::to_string(seq) + "|" + comp;
}

std::string sitekey(const std::string &chain, int seq, const std::string &comp, const std::string &name){
    return residuekey(chain, seq, comp) + "|" + name;
}

float distance2(const atom &first, const atom &second){
    const DirectX::XMVECTOR firstpos = DirectX::XMVectorSet(first.x,first.y,first.z,1.0f);
    const DirectX::XMVECTOR secondpos = DirectX::XMVectorSet(second.x,second.y,second.z,1.0f);
    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(firstpos-secondpos));
}

}

topology readtopology(const std::string &path, const std::vector<atom> &atoms){
    topology result;
    const bondtemplates templates = readtemplates(path);
    std::vector<connection> connections;
    std::ifstream file(path);
    std::unordered_map<std::string, size_t> columns;
    std::string line;
    std::string category;
    bool inloop = false;

    while (std::getline(file, line)){
        if (line == "loop_"){
            columns.clear();
            category.clear();
            inloop = true;
            continue;
        }
        if (line.starts_with("_")){
            if (!inloop) continue;
            const auto header = split(line);
            if (header.empty()) continue;
            columns[header[0]] = columns.size();
            if (header[0].starts_with("_struct_conn.")) category = "connection";
            continue;
        }
        if (line.starts_with("#")){
            inloop = false;
            category.clear();
            continue;
        }

        if (category.empty() || line.empty()) continue;
        const auto row = split(line);

        connection item;
            item.firstchain = field(columns, row, "_struct_conn.ptnr1_label_asym_id");
            item.firstcomp = field(columns, row, "_struct_conn.ptnr1_label_comp_id");
            item.firstatom = field(columns, row, "_struct_conn.ptnr1_label_atom_id");
            item.firstseq = number(field(columns, row, "_struct_conn.ptnr1_label_seq_id"),
            number(field(columns, row, "_struct_conn.ptnr1_auth_seq_id")));
            item.secondchain = field(columns, row, "_struct_conn.ptnr2_label_asym_id");
            item.secondcomp = field(columns, row, "_struct_conn.ptnr2_label_comp_id");
            item.secondatom = field(columns, row, "_struct_conn.ptnr2_label_atom_id");
            item.secondseq = number(field(columns, row, "_struct_conn.ptnr2_label_seq_id"),
            number(field(columns, row, "_struct_conn.ptnr2_auth_seq_id")));

            const std::string type = field(columns, row, "_struct_conn.conn_type_id");
            const bool chemical = type.starts_with("covale") || type.starts_with("disulf") ||
            type.starts_with("metalc") || type.starts_with("modres");

            if (!chemical) continue;
            item.order = type.starts_with("metalc") ? bondorder::coordination :
            bondtype(field(columns, row, "_struct_conn.pdbx_value_order"));
            if (!item.firstchain.empty() && !item.firstatom.empty() &&
                !item.secondchain.empty() && !item.secondatom.empty()) connections.push_back(std::move(item));
    }

    std::unordered_map<std::string, residue> residues;
    std::unordered_map<std::string, uint32_t> sites;
    for (uint32_t i = 0; i < atoms.size(); ++i){
        const atom &a = atoms[i];
        const std::string key = residuekey(a.chain, a.s, a.o);

        auto &residue = residues[key];
        residue.chain = a.chain;
        residue.comp = a.o;
        residue.seq = a.s;
        residue.atoms[a.d] = i;
        sites[sitekey(a.chain, a.s, a.o, a.d)] = i;
    }

    std::unordered_map<uint64_t, size_t> known;
    for (const auto &entry : residues){
        const residue &current = entry.second;
        const auto found = templates.bonds.find(current.comp);

        if (found == templates.bonds.end()) continue;
        for (const templatebond &item : found->second){
            const auto first = current.atoms.find(item.first);
            const auto second = current.atoms.find(item.second);
            if (first == current.atoms.end() || second == current.atoms.end()) continue;
            if (addbond(result, known, first->second, second->second, item.order)) ++result.templatecount;
        }
    }

    for (const connection &item : connections){
        const auto first = sites.find(sitekey(item.firstchain, item.firstseq, item.firstcomp, item.firstatom));
        const auto second = sites.find(sitekey(item.secondchain, item.secondseq, item.secondcomp, item.secondatom));

        if (first == sites.end() || second == sites.end()) continue;
        if (addbond(result, known, first->second, second->second, item.order)) ++result.connectioncount;
    }

    std::unordered_map<std::string, uint32_t> backbone;
    for (uint32_t i = 0; i < atoms.size(); ++i){
        const atom &a = atoms[i];

        if (std::strcmp(a.t, "ATOM") != 0) continue;
        if (std::strcmp(a.d, "C") == 0 || std::strcmp(a.d, "N") == 0)
        backbone[std::string(a.chain) + "|" + std::to_string(a.s) + "|" + a.d] = i;
    }
    for (uint32_t i = 0; i < atoms.size(); ++i){
        const atom &a = atoms[i];
        if (std::strcmp(a.t, "ATOM") != 0 || std::strcmp(a.d, "C") != 0) continue;
        const auto found = backbone.find(std::string(a.chain) + "|" + std::to_string(a.s + 1) + "|N");

        if (found == backbone.end() || distance2(a, atoms[found->second]) > 3.24f) continue;
        if (addbond(result, known, i, found->second, bondorder::single)) ++result.inferredcount;
    }

    std::vector<int> valence(atoms.size(), 0);
    for (const bond &item : result.bonds){
        const int amount = bondweight(item.order);
        valence[item.first] += amount;
        valence[item.second] += amount;
    }

    std::vector<link> inferred = find_links(atoms);
    std::sort(inferred.begin(), inferred.end(), [&](const link &first, const link &second){
        return distance2(atoms[first.first], atoms[first.second]) < distance2(atoms[second.first], atoms[second.second]);
    });
    for (const link &item : inferred){
        const atom &first = atoms[item.first];
        const atom &second = atoms[item.second];
        const bool sameresidue = std::strcmp(first.chain, second.chain) == 0 && first.s == second.s && std::strcmp(first.o, second.o) == 0;
        const auto names = templates.atoms.find(first.o);
        const bool covered = names != templates.atoms.end() && names->second.contains(first.d) && names->second.contains(second.d);

        if (sameresidue && covered) continue;
        if (valence[item.first] >= bondcapacity(first) || valence[item.second] >= bondcapacity(second)) continue;
        if (addbond(result, known, item.first, item.second, bondorder::single)){
            ++result.inferredcount;
            ++valence[item.first];
            ++valence[item.second];
        }
    }
    return result;
}