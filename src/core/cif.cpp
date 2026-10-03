#include "core/cif.hpp"
#include <cctype>
#include <cstring>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

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

std::string value(const std::vector<std::string> &row, int column){
    return column >= 0 && column < static_cast<int>(row.size()) ? row[column] : "";
}

int number(const std::vector<std::string> &row, int column, int fallback){
    const std::string text = value(row, column);
    if (text.empty() || text == "." || text == "?") return fallback;
    try {
        return std::stoi(text);
    }catch (...){
        return fallback;
    }
}

void copy(char *out, size_t size, const std::string &text){
    std::strncpy(out, text.c_str(), size - 1);
}
}

std::vector<atom> ciff::parse(const std::string &cif){
    std::vector<atom> a;
    std::ifstream file(cif);
    if (!file.is_open()) return a;

    std::string line;
    int d = -1, ad = -1, s = -1, as = -1, x = -1, y = -1, z = -1;
    int m = -1, t = -1, i = -1, o = -1, ao = -1, e = -1, c = -1, ac = -1, l = -1;
    int column = 0;
    int first_model = -1;
    bool in_loop = false;
    bool atom_loop = false;
    std::unordered_map<std::string, size_t> sites;

    while (std::getline(file, line)){
        if (line == "loop_"){
            d = ad = s = as = x = y = z = m = t = i = o = ao = e = c = ac = l = -1;
            column = 0;
            in_loop = true;
            atom_loop = false;
            continue;
        }

        if (line.starts_with("_")){
            if (!in_loop) continue;

            const auto header = split(line);
            if (header.empty()) continue;
            const std::string &name = header[0];
            if (name.starts_with("_atom_site.")) atom_loop = true;
            if (name == "_atom_site.group_PDB") t = column;
            if (name == "_atom_site.id") i = column;
            if (name == "_atom_site.type_symbol") e = column;
            if (name == "_atom_site.label_atom_id") d = column;
            if (name == "_atom_site.auth_atom_id") ad = column;
            if (name == "_atom_site.label_alt_id") l = column;
            if (name == "_atom_site.label_comp_id") o = column;
            if (name == "_atom_site.auth_comp_id") ao = column;
            if (name == "_atom_site.label_asym_id") c = column;
            if (name == "_atom_site.auth_asym_id") ac = column;
            if (name == "_atom_site.label_seq_id") s = column;
            if (name == "_atom_site.auth_seq_id") as = column;
            if (name == "_atom_site.Cartn_x") x = column;
            if (name == "_atom_site.Cartn_y") y = column;
            if (name == "_atom_site.Cartn_z") z = column;
            if (name == "_atom_site.pdbx_PDB_model_num") m = column;
            ++column;
            continue;
        }

        if (line.starts_with("#")){
            in_loop = false;
            atom_loop = false;
            continue;
        }

        if (!atom_loop || !(line.starts_with("ATOM ") || line.starts_with("HETATM "))) continue;

        const std::vector<std::string> r = split(line);
        if (x < 0 || y < 0 || z < 0 || x >= static_cast<int>(r.size()) ||
            y >= static_cast<int>(r.size()) || z >= static_cast<int>(r.size())) continue;

        atom ca{};
        try {
            ca.x = std::stof(r[x]);
            ca.y = std::stof(r[y]);
            ca.z = std::stof(r[z]);
        }catch (...){
            continue;
        }

        ca.m = number(r, m, 1);
        if (first_model < 0) first_model = ca.m;
        if (ca.m != first_model) continue;

        ca.i = number(r, i, static_cast<int>(a.size()) + 1);
        ca.s = number(r, s, number(r, as, 0));
        copy(ca.t, sizeof(ca.t), value(r, t));
        auto preferred = [&](int label, int author) {
            std::string result = value(r, label);
            if (result.empty() || result == "." || result == "?") result = value(r, author);
            return result;
        };
        copy(ca.o, sizeof(ca.o), preferred(o, ao));
        copy(ca.d, sizeof(ca.d), preferred(d, ad));
        copy(ca.element, sizeof(ca.element), value(r, e));
        copy(ca.chain, sizeof(ca.chain), preferred(c, ac));
        copy(ca.alt, sizeof(ca.alt), value(r, l));

        const std::string site = std::string(ca.chain) + "|" + std::to_string(ca.s) + "|" + ca.o + "|" + ca.d;
        const auto found = sites.find(site);
        if (found == sites.end()){
            sites.emplace(site, a.size());
            a.push_back(ca);
        }else if (std::strcmp(ca.alt, "A") == 0 || std::strcmp(ca.alt, "1") == 0){
            a[found->second] = ca;
        }
    }
    return a;
}