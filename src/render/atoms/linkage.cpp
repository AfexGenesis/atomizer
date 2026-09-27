#include "render/atoms/atom.hpp"
#include "core/atom/element.hpp"
#include "core/molecule/topology.hpp"
#include <cmath>

namespace {
    DirectX::XMFLOAT4 offset(const atom &first, const atom &second, float amount){
        DirectX::XMVECTOR axis = DirectX::XMVectorSet(second.x-first.x,second.y-first.y,second.z-first.z,0.0f);
        const float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(axis));
        if (length <= 1e-5f) return {0,0,0,0};
        axis = DirectX::XMVectorScale(axis,1.0f/length);

        const DirectX::XMVECTOR ref = std::abs(DirectX::XMVectorGetZ(axis)) >= 0.8f? DirectX::XMVectorSet(0,1,0,0) : DirectX::XMVectorSet(0,0,1,0);
        DirectX::XMVECTOR side = DirectX::XMVector3Cross(axis,ref);
        const float sidelength = DirectX::XMVectorGetX(DirectX::XMVector3Length(side));
        if (sidelength <= 1e-5f) return {0,0,0,0};

    DirectX::XMFLOAT4 result;
    DirectX::XMStoreFloat4(&result,DirectX::XMVectorScale(side,amount/sidelength));
    return result;
}

void addhalf(std::vector<insdata> &instances, const atom &a, const DirectX::XMFLOAT4 &move,
    const DirectX::XMFLOAT4 &middle, float radius){
    DirectX::XMFLOAT4 start, end;
    DirectX::XMStoreFloat4(&start,DirectX::XMVectorSet(a.x,a.y,a.z,1.0f) + DirectX::XMLoadFloat4(&move));
    DirectX::XMStoreFloat4(&end,DirectX::XMLoadFloat4(&middle) + DirectX::XMLoadFloat4(&move));
    start.w = radius;
    end.w = 1.0f;
    instances.push_back({start,style(a).colour,end});
}

void addline(std::vector<insdata> &instances, const atom &first, const atom &second, float amount, float radius){
    const DirectX::XMFLOAT4 move = offset(first, second, amount);
    const DirectX::XMVECTOR start = DirectX::XMVectorSet(first.x,first.y,first.z,1.0f);
    const DirectX::XMVECTOR end = DirectX::XMVectorSet(second.x,second.y,second.z,1.0f);

    DirectX::XMFLOAT4 middle;
    DirectX::XMStoreFloat4(&middle,DirectX::XMVectorLerp(start,end,0.5f));

    addhalf(instances, first, move, middle, radius);
    addhalf(instances, second, move, middle, radius);
}
}

std::vector<insdata> makebondinstances(const std::vector<atom> &atoms, const std::vector<bond> &bonds){
    std::vector<insdata> instances;
    instances.reserve(bonds.size()*4);

    for (const bond &item : bonds){
        if (item.first >= atoms.size() || item.second >= atoms.size()) continue;
        const atom &first = atoms[item.first];
        const atom &second = atoms[item.second];

        if (item.order == bondorder::doublebond){
            addline(instances, first, second, -0.10f, 0.10f);
            addline(instances, first, second, 0.10f, 0.10f);
        }else if (item.order == bondorder::aromatic){
            addline(instances, first, second, -0.09f, 0.11f);
            addline(instances, first, second, 0.09f, 0.055f);
        }else if (item.order == bondorder::triplebond){
            addline(instances, first, second, -0.14f, 0.085f);
            addline(instances, first, second, 0.0f, 0.085f);
            addline(instances, first, second, 0.14f, 0.085f);
        }else{
            const float radius = item.order == bondorder::coordination ? 0.08f : 0.14f;
            addline(instances, first, second, 0.0f, radius);
        }
    }
    return instances;
}