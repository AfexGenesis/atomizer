#include "atominfo.hpp"
#include <cstring>
#include <iomanip>
#include <sstream>
#include <unordered_map>

namespace {
std::string elementname(const std::string &symbol){
    static const std::unordered_map<std::string, std::string> names = {
        {"H", "hydrogen"}, {"D", "deuterium"}, {"C", "carbon"}, {"N", "nitrogen"},
        {"O", "oxygen"}, {"S", "sulfur"}, {"P", "phosphorus"}, {"F", "fluorine"},
        {"CL", "chlorine"}, {"BR", "bromine"}, {"I", "iodine"}, {"FE", "iron"},
        {"ZN", "zinc"}, {"MG", "magnesium"}, {"CA", "calcium"}
    };
        const auto found = names.find(symbol);
        return found == names.end() ? symbol : found->second;
    }  

    std::string position(char code){
        if (code == 'A') return "alpha";
        if (code == 'B') return "beta";
        if (code == 'G') return "gamma";
        if (code == 'D') return "delta";
        if (code == 'E') return "epsilon";
        if (code == 'Z') return "zeta";
        if (code == 'H') return "eta";
        return "";
    }
}

std::string atomrole(const atom &a){
    if (std::strcmp(a.t, "ATOM") != 0) return "";
    const std::string name = a.d;
    const std::string element = a.element;

    if (element == "C" && name == "C") return "carbonyl carbon";
    if (element == "O" && name == "O") return "carbonyl oxygen";
    if (element == "N" && name == "N") return "backbone nitrogen";
    if (name.size() < 2) return "";
    const std::string place = position(name[1]);

    if (place.empty()) return "";
    return place + " " + elementname(element);
}

std::string atomdescription(const atom &a){
    std::ostringstream text;
    text << elementname(a.element) << " | atom " << a.d;
    const std::string role = atomrole(a);

    if (!role.empty()) text << " (" << role << ")";
    text << " | residue " << a.o << ' ' << a.s << " | chain " << a.chain;
    text << " | occupancy " << std::fixed << std::setprecision(2) << a.occupancy;
    text << " | charge ";

    if (a.charge > 0) text << '+';
    text << a.charge;
    return text.str();
}