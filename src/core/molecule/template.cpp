#include "core/molecule/template.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>

namespace {
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
}

bondorder bondtype(std::string value, const std::string &aromatic){
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c){ return std::tolower(c); });
    if (value.starts_with("doub")) return bondorder::doublebond;
    if (value.starts_with("trip")) return bondorder::triplebond;
    if (value.starts_with("sing")) return bondorder::single;
    if (value.starts_with("arom") || value.starts_with("delo") || aromatic == "Y" || aromatic == "y")
        return bondorder::aromatic;
    return bondorder::single;
}

int bondrank(bondorder order){
    if (order == bondorder::triplebond) return 4;
    if (order == bondorder::doublebond) return 3;
    if (order == bondorder::aromatic) return 2;
    if (order == bondorder::single) return 1;
    return 0;
}

int bondweight(bondorder order){
    if (order == bondorder::doublebond) return 2;
    if (order == bondorder::triplebond) return 3;
    if (order == bondorder::coordination) return 0;
    return 1;
}

int bondcapacity(const atom &a){
    const std::string element = a.element;
    if (element == "H" || element == "D") return 1;
    if (element == "C") return 4;
    if (element == "N") return 4;
    if (element == "O") return 2;
    if (element == "F" || element == "CL" || element == "BR" || element == "I") return 1;
    if (element == "S" || element == "P") return 6;
    return 8;
}

bondtemplates readtemplates(const std::string &path){
    bondtemplates result;
    std::ifstream file(path);
    std::unordered_map<std::string, size_t> columns;
    std::string line;
    bool inloop = false;
    bool inbonds = false;

    while (std::getline(file, line)){
        if (line == "loop_"){
            columns.clear();
            inloop = true;
            inbonds = false;
            continue;
        }
        if (line.starts_with("_")){
            if (!inloop) continue;
            const auto header = split(line);
            if (header.empty()) continue;
            columns[header[0]] = columns.size();
            if (header[0].starts_with("_chem_comp_bond.")) inbonds = true;
            continue;
        }
        if (line.starts_with("#")){
            inloop = false;
            inbonds = false;
            continue;
        }
        if (!inbonds || line.empty()) continue;

        const auto row = split(line);
        const std::string comp = field(columns, row, "_chem_comp_bond.comp_id");
        const std::string first = field(columns, row, "_chem_comp_bond.atom_id_1");
        const std::string second = field(columns, row, "_chem_comp_bond.atom_id_2");
        if (comp.empty() || first.empty() || second.empty()) continue;

        const std::string value = field(columns, row, "_chem_comp_bond.value_order");
        const std::string aromatic = field(columns, row, "_chem_comp_bond.pdbx_aromatic_flag");
        result.bonds[comp].push_back({first, second, bondtype(value, aromatic)});
        result.atoms[comp].insert(first);
        result.atoms[comp].insert(second);
    }
    return result;
}