#pragma once
#include "../cif.hpp"
#include <cstdint>
#include <vector>

struct link {
    uint32_t first;
    uint32_t second;
};

std::vector<link> find_links(const std::vector<atom> &atoms);