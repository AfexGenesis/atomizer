#include "render/model/model.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace {
using vec = DirectX::XMVECTOR;
vec point(const DirectX::XMFLOAT4 &p){ return DirectX::XMLoadFloat4(&p); }
vec cross(vec a, vec b){ return DirectX::XMVector3Cross(a,b); }
float dot(vec a, vec b){ return DirectX::XMVectorGetX(DirectX::XMVector3Dot(a,b)); }

bool hits_box(const modelpiece &piece, const DirectX::XMFLOAT4 &origin, const DirectX::XMFLOAT4 &direction, float best){
    float near = 0.0f;
    float far = best;
    const float starts[4] = {origin.x,origin.y,origin.z,origin.w};
    const float rays[4] = {direction.x,direction.y,direction.z,direction.w};
    const float minimum[4] = {piece.minimum.x,piece.minimum.y,piece.minimum.z,piece.minimum.w};
    const float maximum[4] = {piece.maximum.x,piece.maximum.y,piece.maximum.z,piece.maximum.w};
    for (int axis = 0; axis < 3; ++axis){
        if (std::abs(rays[axis]) < 1e-7f){
            if (starts[axis] < minimum[axis] || starts[axis] > maximum[axis]) return false;
            continue;
        }
        float a = (minimum[axis]-starts[axis])/rays[axis];
        float b = (maximum[axis]-starts[axis])/rays[axis];
        if (a > b) std::swap(a,b);
        near = std::max(near,a);
        far = std::min(far,b);
        if (far < near) return false;
    }
    return true;
}
}

modelhit pmodel(const modelmesh &mesh, const DirectX::XMFLOAT4 &origin, const DirectX::XMFLOAT4 &direction){
    modelhit hit;
    float best = std::numeric_limits<float>::max();
    const vec start = DirectX::XMLoadFloat4(&origin);
    const vec ray = DirectX::XMLoadFloat4(&direction);

    for (size_t part = 0; part < mesh.pieces.size(); ++part){
        const auto &piece = mesh.pieces[part];
        if (!hits_box(piece,origin,direction,best)) continue;
        const size_t end = static_cast<size_t>(piece.firstindex) + piece.countindex;
        for (size_t i = piece.firstindex; i + 2 < end; i += 3){
            const vec a = point(mesh.vertices[mesh.indices[i]].position);
            const vec b = point(mesh.vertices[mesh.indices[i+1]].position);
            const vec c = point(mesh.vertices[mesh.indices[i+2]].position);
            const vec edge1 = b-a, edge2 = c-a;
            const vec p = cross(ray,edge2);
            const float det = dot(edge1,p);

            if (std::abs(det) < 1e-7f) continue;
            const float inverse = 1.0f/det;
            const vec offset = start-a;
            const float u = dot(offset,p)*inverse;

            if (u < 0.0f || u > 1.0f) continue;
            const vec q = cross(offset,edge1);
            const float v = dot(ray,q)*inverse;

            if (v < 0.0f || u+v > 1.0f) continue;
            const float distance = dot(edge2,q)*inverse;

            if (distance <= 0.0f || distance >= best) continue;
            best = distance;
            hit.piece = static_cast<int>(part);

            DirectX::XMStoreFloat4(&hit.position,start + ray*distance);
        }
    }
    return hit;
}