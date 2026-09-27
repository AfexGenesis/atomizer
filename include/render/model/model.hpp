#pragma once
#include "../../core/cif.hpp"
#include "../../core/molecule/module.hpp"
#include <algorithm>
#include <DirectXMath.h>
#include <cstdint>
#include <vector>

struct modelvertex {
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 normal;
    DirectX::XMFLOAT4 colour;
};

struct modelpiece {
    uint32_t firstindex = 0;
    uint32_t countindex = 0;
    std::string chain;
    
    int firstresidue = 0;
    int lastresidue = 0;
    shape type = shape::coil;
    DirectX::XMFLOAT4 minimum = {};
    DirectX::XMFLOAT4 maximum = {};
};

struct modelmesh {
    std::vector<modelvertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<modelpiece> pieces;
};

struct modelhit {
    int piece = -1;
    DirectX::XMFLOAT4 position = {};
};

modelmesh mmodel(const std::vector<atom> &atoms, const std::vector<segment> &segments);
modelhit pmodel(const modelmesh &mesh, const DirectX::XMFLOAT4 &origin, const DirectX::XMFLOAT4 &direction);