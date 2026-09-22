#include "render/atoms/atom.hpp"
#include "core/atom/element.hpp"
#include "core/molecule/topology.hpp"
#include <cmath>

namespace {
struct point {
    float x;
    float y;
    float z;
};

point offset(const atom &first, const atom &second, float amount){
    float x = second.x-first.x;
    float y = second.y-first.y;
    float z = second.z-first.z;
    const float length = std::sqrt(x*x+y*y+z*z);

    if (length <= 1e-5f) return {0,0,0};
    x /= length;
    y /= length;
    z /= length;
    float ry = 0.0f;
    float rz = 1.0f;

    if (std::abs(z) >= 0.8f){
        ry = 1.0f;
        rz = 0.0f;
    }

    float px = y*rz-z*ry;
    float py = -x*rz;
    float pz = x*ry;
    const float plen = std::sqrt(px*px+py*py+pz*pz);

    if (plen <= 1e-5f) return {0,0,0};
    return {px*amount/plen, py*amount/plen, pz*amount/plen};
}

void addhalf(std::vector<insdata> &instances, const atom &a, const point &move,
    const DirectX::XMFLOAT4 &middle, float radius){
    instances.push_back({{a.x+move.x, a.y+move.y, a.z+move.z, radius}, style(a).colour,
    { middle.x+move.x, middle.y+move.y, middle.z+move.z, 1}});
}

void addline(std::vector<insdata> &instances, const atom &first, const atom &second, float amount, float radius){
    const point move = offset(first, second, amount);
    const DirectX::XMFLOAT4 middle{(first.x+second.x)*0.5f, (first.y+second.y)*0.5f,
    (first.z+second.z)*0.5f, 1};

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