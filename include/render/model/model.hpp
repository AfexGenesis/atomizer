#pragma once
#include "../../core/cif.hpp"
#include "../../core/molecule/module.hpp"
#include <cstdint>
#include <vector>

struct modelvertex {
    float position[3];
    float normal[3];
    float colour[4];
};

struct modelpiece {
    uint32_t firstindex = 0;
    uint32_t countindex = 0;
    std::string chain;
    
    int firstresidue = 0;
    int lastresidue = 0;
    shape type = shape::coil;
    float minimum[3] = {};
    float maximum[3] = {};
};

struct modelmesh {
    std::vector<modelvertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<modelpiece> pieces;
};

struct modelhit {
    int piece = -1;
    float position[3] = {};
};

modelmesh mmodel(const std::vector<atom> &atoms, const std::vector<segment> &segments);
modelhit pmodel(const modelmesh &mesh, const float origin[3], const float direction[3]);