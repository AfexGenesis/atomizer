#pragma once
#include "../cif.hpp"
#include <DirectXMath.h>

struct elementstylied {
    float covalent;
    float spacefill;
    DirectX::XMFLOAT4 colour;
};

elementstylied style(const atom &a);