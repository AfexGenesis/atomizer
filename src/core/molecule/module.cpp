#include "core/molecule/module.hpp"
#include <cctype>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {
    std::vector<std::string> split(const std::string &line){
        std::vector<std::string> values;
        for (size_t i = 0; i < line.size();){
            if (std::isspace(static_cast<unsigned char>(line[i]))){ ++i; continue; }
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

std::vector<segment> readmodule(const std::string &path){
    std::vector<segment> segments;
    std::ifstream file(path);
    std::unordered_map<std::string, size_t> columns;
    std::string line;
    bool in_loop = false;
    std::string category;

    while (std::getline(file, line)){
        if (line == "loop_"){
            columns.clear();
            category.clear();
            in_loop = true;
            continue;
        }

        if (line.starts_with("_")){
            if (in_loop){
                auto fields = split(line);
                if (!fields.empty()){
                    columns[fields[0]] = columns.size();
                    if (fields[0].starts_with("_struct_conf.")) category = "_struct_conf.";
                    if (fields[0].starts_with("_struct_sheet_range.")) category = "_struct_sheet_range.";
                }
            }
            continue;
        }

        if (line.starts_with("#")){ in_loop = false; category.clear(); continue; }
        if (category.empty() || line.empty()) continue;
        auto row = split(line);
        auto field = [&](const char *name) -> std::string {
            const auto it = columns.find(category + name);
            return it != columns.end() && it->second < row.size() ? row[it->second] : "";
        };
        auto preferred = [&](const char *label, const char *author) -> std::string {
            std::string result = field(label);
            if (result.empty() || result == "." || result == "?") result = field(author);
            return result;
        };

        shape type = shape::sheet;
        if (category == "_struct_conf."){
            if (!field("conf_type_id").starts_with("HELX")) continue;
            type = shape::helix;
        }try{
            segment s;
            s.chain = preferred("beg_label_asym_id", "beg_auth_asym_id");
            if (s.chain.empty() || s.chain != preferred("end_label_asym_id", "end_auth_asym_id")) continue;
            s.first = std::stoi(preferred("beg_label_seq_id", "beg_auth_seq_id"));
            s.last = std::stoi(preferred("end_label_seq_id", "end_auth_seq_id"));
            s.type = type;
            if (s.first <= s.last) segments.push_back(std::move(s));
        }catch (...){ continue; }
    }
    return segments;
}