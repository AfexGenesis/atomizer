#include <cmath>
#include "grid.hpp"

void grid::build(std::vector<atom> atoms, float size){
    clear();
    sizee = size;
    a.reserve(atoms.size());

    for (const auto& item : atoms){
        atom ga = item;

        ga.cx = static_cast<int>(std::floor(item.x / sizee));
        ga.cy = static_cast<int>(std::floor(item.y / sizee));
        ga.cz = static_cast<int>(std::floor(item.z / sizee));

        a.push_back(ga);
    }
}

void grid::clear(){
    a.clear();
}