#pragma once
#include <vector>
#include <cstdint>
#include <DirectXMath.h>

struct atomertex{
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 normal;
    DirectX::XMFLOAT4 colour;
};

class atom{
    public:
        atom(float radius = 1.0f, uint32_t statcks = 64, uint32_t slices = 64,
            const DirectX::XMFLOAT4 &col = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
            bool rgb = true);
        const std::vector<atomertex> &verti() const{
            return vertices;
        }
        const std::vector<uint32_t> &indi() const{
            return indices;
        }

    private:
        void generation(float radius, uint32_t stacks, uint32_t slices, const DirectX::XMFLOAT4 &col, bool rgb);
        std::vector<atomertex> vertices;
        std::vector<uint32_t> indices;
};