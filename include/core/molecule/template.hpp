#pragma once
#include "topology.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct templatebond {
    std::string first;
    std::string second;
    bondorder order = bondorder::single;
};

struct bondtemplates {
    std::unordered_map<std::string, std::vector<templatebond>> bonds;
    std::unordered_map<std::string, std::unordered_set<std::string>> atoms;
};

bondtemplates readtemplates(const std::string &path);
bondorder bondtype(std::string value, const std::string &aromatic = "");
int bondrank(bondorder order);
int bondweight(bondorder order);
int bondcapacity(const atom &a);