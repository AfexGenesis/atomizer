
#include "atom.hpp"
#include "atom/element.hpp"

atomized::atomized(){
    vertices = {
        {{-1.0f, -1.0f}},
        {{ 1.0f, -1.0f}},
        {{ 1.0f,  1.0f}},
        {{-1.0f,  1.0f}}
    };
    indices = {0, 1, 2, 0, 2, 3};
}

std::vector<insdata> make_atom_instances(const std::vector<atom> &atoms, bool spacefill){
    std::vector<insdata> instances;
    instances.reserve(atoms.size());
    for (const auto &a : atoms){
        const auto look = style(a);
        const float radius = spacefill ? look.spacefill : look.covalent * 0.40f;
        instances.push_back({{a.x, a.y, a.z, radius}, look.colour, {0, 0, 0, 0}});
    }
    return instances;
}