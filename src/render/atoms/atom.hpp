#pragma once
#include <vector>
#include <cstdint>
#include <DirectXMath.h>

struct atomertex{
    DirectX::XMFLOAT2 corner;
};

struct insdata{
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 colour;
};

class atomized{
    public:
        atomized();
        const std::vector<atomertex> &verti() const{
            return vertices;
        }
        const std::vector<uint32_t> &indi() const{
            return indices;
        }

    private:
        std::vector<atomertex> vertices;
        std::vector<uint32_t> indices;
};