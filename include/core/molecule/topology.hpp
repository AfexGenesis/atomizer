#pragma once
#include "../cif.hpp"
#include <cstdint>
#include <string>
#include <vector>

enum class bondorder {
    single,
    doublebond,
    triplebond,
    aromatic,
    coordination
};

struct bond {
    uint32_t first = 0;
    uint32_t second = 0;
    bondorder order = bondorder::single;
};

struct topology {
    std::vector<bond> bonds;
    size_t templatecount = 0;
    size_t connectioncount = 0;
    size_t inferredcount = 0;
};

topology readtopology(const std::string &path, const std::vector<atom> &atoms);