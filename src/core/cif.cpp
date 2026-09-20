#include "cif.hpp"
#include <cctype>
#include <cstring>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {
void copy(char *out, size_t size, const std::string &value){
    std::strncpy(out, value.c_str(), size - 1);
}

std::vector<std::string> split(const std::string &line){
    std::vector<std::string> values;
    for (size_t i = 0; i < line.size();){
        if (std::isspace(static_cast<unsigned char>(line[i]))){ ++i; continue; }
        if (line[i] == '#' && values.empty()) break;
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
}

std::vector<atom> ciff::parse(const std::string &path){
    std::vector<atom> atoms;
    std::ifstream file(path);
    std::unordered_map<std::string, size_t> columns;
    std::string line;
    bool in_loop = false;
    bool atom_loop = false;
    int first_model = -1;
    std::unordered_map<std::string, size_t> sites;

    while (std::getline(file, line)){
        if (line == "loop_"){
            columns.clear();
            in_loop = true;
            atom_loop = false;
            continue;
        }
        if (line.starts_with("_")){
            if (in_loop){
                auto names = split(line);
                if (!names.empty()){
                    columns[names[0]] = columns.size();
                    atom_loop |= names[0].starts_with("_atom_site.");
                }
            }
            continue;
        }
        if (line.starts_with("#")){
            in_loop = false;
            atom_loop = false;
            continue;
        }
        if (!atom_loop || !(line.starts_with("ATOM ") || line.starts_with("HETATM "))) continue;
        auto row = split(line);
        auto field = [&](const char *name) -> std::string {
            auto it = columns.find(name);
            return it != columns.end() && it->second < row.size() ? row[it->second] : "";
        };
        auto number = [&](const char *name, int fallback) {
            auto value = field(name);
            if (value.empty() || value == "." || value == "?") return fallback;
            try { return std::stoi(value); } catch (...) { return fallback; }
        };
        auto decimal = [&](const char *name, float fallback) {
            auto value = field(name);
            if (value.empty() || value == "." || value == "?") return fallback;
            try { return std::stof(value); } catch (...) { return fallback; }
        };
        const int model = number("_atom_site.pdbx_PDB_model_num", 1);
        if (first_model < 0) first_model = model;
        if (model != first_model) continue;
        auto alt = field("_atom_site.label_alt_id");

        atom a;
        try {
            a.x = std::stof(field("_atom_site.Cartn_x"));
            a.y = std::stof(field("_atom_site.Cartn_y"));
            a.z = std::stof(field("_atom_site.Cartn_z"));
        } catch (...) { continue; }
        a.i = number("_atom_site.id", static_cast<int>(atoms.size()) + 1);
        a.s = number("_atom_site.label_seq_id", number("_atom_site.auth_seq_id", 0));
        a.m = model;
        a.occupancy = decimal("_atom_site.occupancy", 1.0f);
        a.charge = number("_atom_site.pdbx_formal_charge", 0);
        copy(a.t, sizeof(a.t), field("_atom_site.group_PDB"));
        copy(a.d, sizeof(a.d), field("_atom_site.label_atom_id"));
        copy(a.o, sizeof(a.o), field("_atom_site.label_comp_id"));
        copy(a.element, sizeof(a.element), field("_atom_site.type_symbol"));
        copy(a.chain, sizeof(a.chain), field("_atom_site.label_asym_id"));
        copy(a.alt, sizeof(a.alt), alt);
        const std::string site = std::string(a.chain) + "|" + std::to_string(a.s) + "|" +
                                 a.o + "|" + a.d;
        auto found = sites.find(site);
        if (found == sites.end()){
            sites.emplace(site, atoms.size());
            atoms.push_back(a);
        }else if (alt == "A" || alt == "1"){
            atoms[found->second] = a;
        }
    }
    return atoms;
}