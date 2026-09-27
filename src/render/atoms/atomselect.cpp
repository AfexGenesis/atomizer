#include "render/atoms/atomselect.hpp"
#include "core/atom/element.hpp"
#include <cmath>
#include <limits>

atomhit patom(const std::vector<atom>
    &atoms, const DirectX::XMFLOAT4 &origin, const DirectX::XMFLOAT4 &direction, bool spacefill){
    atomhit hit;
    float best = std::numeric_limits<float>::max();
    const DirectX::XMVECTOR start = DirectX::XMLoadFloat4(&origin);
    const DirectX::XMVECTOR ray = DirectX::XMLoadFloat4(&direction);

    for (size_t i = 0; i < atoms.size(); ++i){
        const atom &a = atoms[i];
        const auto look = style(a);
        const float radius = spacefill ? look.spacefill : look.covalent * 0.40f;
        const DirectX::XMVECTOR center = DirectX::XMVectorSet(a.x,a.y,a.z,1.0f);
        const DirectX::XMVECTOR delta = start - center;
        const float along = DirectX::XMVectorGetX(DirectX::XMVector3Dot(delta,ray));
        const float length = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(delta));
        const float det = along*along - (length - radius*radius);

        if (det < 0.0f) continue;
        const float root = std::sqrt(det);
        float distance = -along - root;

        if (distance <= 0.0f) distance = -along + root;
        if (distance <= 0.0f || distance >= best) continue;

        best = distance;
        hit.index = static_cast<int>(i);
        hit.distance = distance;
    }
    return hit;
}