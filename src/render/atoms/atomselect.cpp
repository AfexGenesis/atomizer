#include "atomselect.hpp"
#include "atom/element.hpp"
#include <cmath>
#include <limits>

atomhit patom(const std::vector<atom>
    &atoms, const float origin[3], const float direction[3], bool spacefill){
    atomhit hit;
    float best = std::numeric_limits<float>::max();

    for (size_t i = 0; i < atoms.size(); ++i){
        const atom &a = atoms[i];
        const auto look = style(a);
        const float radius = spacefill ? look.spacefill : look.covalent * 0.40f;
        const float x = origin[0] - a.x;
        const float y = origin[1] - a.y;
        const float z = origin[2] - a.z;
        const float along = x*direction[0] + y*direction[1] + z*direction[2];
        const float det = along*along - (x * x + y *y + z * z - radius*radius);

        if (det < 0.0f) continue;
        const float root = std::sqrt(det);
        float distance = -along - root;

        if (distance <= 0.0f) distance = -along + root;
        if (distance <= 0.0f || distance >= best) continue;

        best = distance;
        hit.index = static_cast<int>(i);
        hit.distance = distance;
    }
    return hit;
}