#include "render/model/model.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace {
struct vec { float x, y, z; };
vec point(const float p[3]){ return {p[0],p[1],p[2]}; }
vec operator-(vec a, vec b){ return {a.x-b.x,a.y-b.y,a.z-b.z}; }
vec cross(vec a, vec b){ return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; }
float dot(vec a, vec b){ return a.x*b.x+a.y*b.y+a.z*b.z; }

bool hits_box(const modelpiece &piece, const float origin[3], const float direction[3], float best){
    float near = 0.0f;
    float far = best;
    for (int axis = 0; axis < 3; ++axis){
        if (std::abs(direction[axis]) < 1e-7f){
            if (origin[axis] < piece.minimum[axis] || origin[axis] > piece.maximum[axis]) return false;
            continue;
        }
        float a = (piece.minimum[axis]-origin[axis])/direction[axis];
        float b = (piece.maximum[axis]-origin[axis])/direction[axis];
        if (a > b) std::swap(a,b);
        near = std::max(near,a);
        far = std::min(far,b);
        if (far < near) return false;
    }
    return true;
}
}

modelhit pmodel(const modelmesh &mesh, const float origin[3], const float direction[3]){
    modelhit hit;
    float best = std::numeric_limits<float>::max();
    const vec start = point(origin);
    const vec ray = point(direction);

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

            for (int axis = 0; axis < 3; ++axis)
                hit.position[axis] = origin[axis] + direction[axis]*distance;
        }
    }
    return hit;
}