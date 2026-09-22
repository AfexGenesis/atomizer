#pragma once
#include <string>
#include <vector>

enum class shape { coil, helix, sheet };

struct segment {
    std::string chain;
    int first = 0;
    int last = 0;
    shape type = shape::coil;
};

std::vector<segment> read_secondary(const std::string &path);