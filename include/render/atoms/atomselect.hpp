#pragma once
#include "core/cif.hpp"
#include <vector>
#include <DirectXMath.h>

struct atomhit {
    int index = -1;
    float distance = 0.0f;
};

atomhit patom(const std::vector<atom> &atoms, const DirectX::XMFLOAT4 &origin, const DirectX::XMFLOAT4 &direction, bool spacefill);