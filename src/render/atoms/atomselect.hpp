#pragma once
#include "cif.hpp"
#include <vector>

struct atomhit {
    int index = -1;
    float distance = 0.0f;
};

atomhit patom(const std::vector<atom> &atoms, const float origin[3], const float direction[3], bool spacefill);