#include <print>
#include "core/cif.hpp"
#include "core/grid.hpp"
#include "core/parser.hpp"

moleculedata molecarser(const std::string &path){
    moleculedata data;
    if(path.empty()) return data;

    ciff parser;
    std::vector<atom> a = parser.parse(path);
    if (a.empty()) return data;

    grid ag;
    data.segments = readmodule(path);
    topology chemistry = readtopology(path, a);
    data.bonds = std::move(chemistry.bonds);
    ag.build(std::move(a));
    std::println("loaded {} atoms and {} bonds", ag.count(), chemistry.bonds.size());
    std::println("bond sources: {} component, {} structure, {} fallback", chemistry.templatecount, chemistry.connectioncount, chemistry.inferredcount);
    return data;
}