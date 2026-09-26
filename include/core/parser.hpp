#pragma once
#include <string>
#include <vector>
#include "core/cif.hpp"
#include "core/molecule/module.hpp"
#include "core/molecule/topology.hpp"

struct moleculedata{
    std::vector<atom> atoms;
    std::vector<segment> segments;
    std::vector<bond> bonds;
    bool valid = false;
};

moleculedata molecarser(const std::string &path);