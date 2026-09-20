#include "linkage.hpp"
#include "../atom/element.hpp"
#include <cmath>
#include <cstring>
#include <unordered_map>

namespace {
struct cell {
    int x, y, z;
    bool operator==(const cell &) const = default;
};
struct hash {
    size_t operator()(cell c) const {
        size_t value = 1469598103934665603ull;
        for (int part : {c.x, c.y, c.z}) value = (value ^ static_cast<uint32_t>(part)) * 1099511628211ull;
        return value;
    }
};
cell location(const atom &a){
    return {static_cast<int>(std::floor(a.x / 2.5f)),
            static_cast<int>(std::floor(a.y / 2.5f)),
            static_cast<int>(std::floor(a.z / 2.5f))};
}
bool related(const atom &a, const atom &b){
    const bool firsthydrogen = std::strcmp(a.element, "H") == 0 || std::strcmp(a.element, "D") == 0;
    const bool secondhydrogen = std::strcmp(b.element, "H") == 0 || std::strcmp(b.element, "D") == 0;
    if (firsthydrogen && secondhydrogen) return false;

    const bool sulfur = std::strcmp(a.element, "S") == 0 && std::strcmp(b.element, "S") == 0 &&
    std::strcmp(a.d, "SG") == 0 && std::strcmp(b.d, "SG") == 0;

    if (sulfur) return true;
    if (std::strcmp(a.chain, b.chain) != 0) return false;
    if (a.s == b.s) return std::strcmp(a.o, b.o) == 0;
    return a.s > 0 && b.s > 0 && std::abs(a.s - b.s) == 1 &&
        ((std::strcmp(a.d, "C") == 0 && std::strcmp(b.d, "N") == 0) ||
         (std::strcmp(a.d, "N") == 0 && std::strcmp(b.d, "C") == 0));
}
}

std::vector<link> find_links(const std::vector<atom> &atoms){
    std::vector<link> links;
    std::unordered_map<cell, std::vector<uint32_t>, hash> cells;
    for (uint32_t i = 0; i < atoms.size(); ++i){
        const auto &a = atoms[i];
        const cell current = location(a);
        for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y)
        for (int z = -1; z <= 1; ++z){
            auto it = cells.find({current.x + x, current.y + y, current.z + z});
            if (it == cells.end()) continue;
            for (uint32_t j : it->second){
                const auto &b = atoms[j];
                if (!related(a, b)) continue;
                const float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
                const float length2 = dx * dx + dy * dy + dz * dz;
                const float limit = std::min(style(a).covalent + style(b).covalent + 0.40f, 2.35f);
                if (length2 > 0.16f && length2 <= limit * limit) links.push_back({j, i});
            }
        }
        cells[current].push_back(i);
    }
    return links;
}