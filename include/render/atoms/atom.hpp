#pragma once
#include <vector>
#include <cstdint>
#include "core/cif.hpp"
#include "core/molecule/topology.hpp"
#include <DirectXMath.h>

struct atomertex{
    DirectX::XMFLOAT2 corner;
};

struct insdata{
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 colour;
    DirectX::XMFLOAT4 end;
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

std::vector<insdata> make_atom_instances(const std::vector<atom> &atoms, bool spacefill);
std::vector<insdata> makebondinstances(const std::vector<atom> &atoms, const std::vector<bond> &bonds);