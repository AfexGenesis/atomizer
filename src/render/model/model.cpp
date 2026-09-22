#include "render/model/model.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace {
    struct vec { float x = 0, y = 0, z = 0; };
    vec operator+(vec a, vec b){ return {a.x + b.x, a.y + b.y, a.z + b.z}; }
    vec operator-(vec a, vec b){ return {a.x - b.x, a.y - b.y, a.z - b.z}; }
    vec operator*(vec a, float n){ return {a.x * n, a.y * n, a.z * n}; }
    float dot(vec a, vec b){ return a.x * b.x + a.y * b.y + a.z * b.z; }
    vec cross(vec a, vec b){ return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x}; }
    vec unit(vec a){ const float n = std::sqrt(dot(a,a)); return n > 1e-5f ? a * (1.0f/n) : vec{0,1,0}; }
    vec point(const atom &a){ return {a.x, a.y, a.z}; }
    struct residue { std::string chain; int seq = 0; vec ca, c, n; bool has_ca = false, has_c = false, has_n = false; shape type = shape::coil; };
    float width(shape type){ return type == shape::sheet ? 0.68f : type == shape::helix ? 0.55f : 0.20f; }
    float thick(shape type){ return type == shape::sheet ? 0.10f : type == shape::helix ? 0.22f : 0.20f; }
    vec colour(shape type){ return type == shape::sheet ? vec{0.95f,0.78f,0.22f} : type == shape::helix ? vec{0.87f,0.29f,0.32f} : vec{0.63f,0.70f,0.78f}; }

    vec spline(vec a, vec b, vec c, vec d, float t){
        const float t2 = t*t, t3 = t2*t;
        return (b*2.0f + (c-a)*t + (a*2.0f-b*5.0f+c*4.0f-d)*t2 + (b*3.0f-a-c*3.0f+d)*t3)*0.5f;
    }

    void addrun(modelmesh &mesh, const std::vector<residue> &run, size_t first, size_t last, shape type){
        if (last <= first) return;
        const size_t first_vertex = mesh.vertices.size();
        const size_t firstindex = mesh.indices.size();
        constexpr int sides = 8;
        constexpr int steps = 5;
        constexpr float pi = 3.14159265358979323846f;
        bool lastprevious = false;

        uint32_t previous = 0;
        vec lastside{};
        vec sheetside{};

        if (type == shape::sheet){

            for (size_t i = first+1; i < last; ++i){
                vec zig = run[i].ca - (run[i-1].ca + run[i+1].ca)*0.5f;
                if (dot(zig,zig) < 1e-4f) continue;
                zig = unit(zig);
                if (dot(zig,sheetside) < 0.0f) zig = zig * -1.0f;
                sheetside = sheetside + zig;
            }
            if (dot(sheetside,sheetside) < 1e-4f && run[first].has_c)
                sheetside = run[first].c - run[first].ca;
            sheetside = unit(sheetside);
        }
        for (size_t i = first; i < last; ++i){
            const vec a = run[i > first ? i-1 : i].ca;
            const vec b = run[i].ca;
            const vec c = run[i+1].ca;
            const vec d = run[i+2 <= last ? i+2 : i+1].ca;
            for (int step = 0; step <= steps; ++step){
                if (i > first && step == 0) continue;
                const float t = static_cast<float>(step) / steps;
                const vec center = spline(a,b,c,d,t);
                const vec before = spline(a,b,c,d,std::max(0.0f,t-0.01f));
                const vec after = spline(a,b,c,d,std::min(1.0f,t+0.01f));
                const vec tangent = unit(after-before);
                vec side;

                if (type == shape::sheet){
                    side = sheetside - tangent*dot(sheetside,tangent);
                    if (dot(side,side) < 1e-4f && lastprevious)
                        side = lastside - tangent*dot(lastside,tangent);
                    if (dot(side,side) < 1e-4f)
                        side = cross(tangent,vec{0,1,0});
                    if (dot(side,side) < 1e-4f)
                        side = cross(tangent,vec{1,0,0});
                    side = unit(side);
                    if (lastprevious && dot(side,lastside) < 0.0f) side = side * -1.0f;
                }else if (!lastprevious){
                    vec ref = run[i].has_c ? run[i].c-run[i].ca : vec{0,1,0};
                    side = unit(cross(tangent,ref));
                    if (dot(side,side) < 0.5f || dot(cross(tangent,ref),cross(tangent,ref)) < 1e-4f)
                        side = unit(cross(tangent,vec{0,1,0}));
                }else{
                    side = unit(lastside - tangent*dot(lastside,tangent));
                }

                if (dot(side,side) < 0.5f) side = unit(cross(tangent,vec{1,0,0}));
                lastside = side;

                const vec up = unit(cross(tangent,side));
                float w = width(type);
                float h = thick(type);
                const vec col = colour(type);

                if (type == shape::sheet && i+1 == last){
                   w *= t < 0.25f ? 1.0f + 0.5f*t/0.25f : std::max(0.02f,1.5f*(1.0f-t)/0.75f);
                    if (t > 0.85f) h *= std::max(0.10f,(1.0f-t)/0.15f);
                }
                
                const uint32_t ring = static_cast<uint32_t>(mesh.vertices.size());
                for (int j = 0; j < sides; ++j){
                    vec pos, normal;
                    if (type == shape::sheet){
                        const float across[] = {1,-1,-1,-1,-1,1,1,1};
                        const float height[] = {1,1,1,-1,-1,-1,-1,1};
                        pos = center + side*(w*across[j]) + up*(h*height[j]);
                        normal = j < 2 ? up : j < 4 ? side*-1.0f :
                                 j < 6 ? up*-1.0f : side;
                    }else{
                        const float angle = 2*pi*j/sides;
                        const float cs = std::cos(angle), sn = std::sin(angle);
                        pos = center + side*(w*cs) + up*(h*sn);
                        normal = unit(side*(cs/w) + up*(sn/h));
                    }
                mesh.vertices.push_back({{pos.x,pos.y,pos.z}, {normal.x,normal.y,normal.z}, {col.x,col.y,col.z,1.0f}});
                }

            if (lastprevious){
                for (int j = 0; j < sides; ++j){
                    const uint32_t next = (j+1)%sides;
                    mesh.indices.insert(mesh.indices.end(), {previous+static_cast<uint32_t>(j), ring+static_cast<uint32_t>(j), ring+next,
                    previous+static_cast<uint32_t>(j), ring+next, previous+next});
                }
            }
            previous = ring;
            lastprevious = true;
        }
    }
        modelpiece piece;
        piece.firstindex = static_cast<uint32_t>(firstindex);
        piece.countindex = static_cast<uint32_t>(mesh.indices.size() - firstindex);
        piece.chain = run[first].chain;
        piece.firstresidue = run[first].seq;
        piece.lastresidue = run[last].seq;
        piece.type = type;

        for (int axis = 0; axis < 3; ++axis){
            piece.minimum[axis] = mesh.vertices[first_vertex].position[axis];
            piece.maximum[axis] = piece.minimum[axis];
            for (size_t i = first_vertex+1; i < mesh.vertices.size(); ++i){
                piece.minimum[axis] = std::min(piece.minimum[axis], mesh.vertices[i].position[axis]);
                piece.maximum[axis] = std::max(piece.maximum[axis], mesh.vertices[i].position[axis]);
            }
        }
        mesh.pieces.push_back(std::move(piece));
    }

    void addchain(modelmesh &mesh, const std::vector<residue> &run){
        if (run.size() < 2) return;
        auto interval = [&](size_t i){ return run[i].type == run[i+1].type ? run[i].type : shape::coil; };
        size_t first = 0;
        shape type = interval(0);

        for (size_t i = 1; i+1 < run.size(); ++i){
            const shape next = interval(i);
            if (next == type) continue;
            addrun(mesh,run,first,i,type);
            first = i;
            type = next;
        }
        addrun(mesh,run,first,run.size()-1,type);
    }
}

modelmesh mmodel(const std::vector<atom> &atoms, const std::vector<segment> &segments){
    std::map<std::pair<std::string,int>,residue> residues;
    for (const auto &a : atoms){
        if (std::strcmp(a.t,"ATOM") != 0 || a.s <= 0) continue;
        auto &r = residues[{a.chain,a.s}];
        r.chain = a.chain;
        r.seq = a.s;
        if (std::strcmp(a.d,"CA") == 0){ r.ca = point(a); r.has_ca = true; }
        if (std::strcmp(a.d,"C") == 0){ r.c = point(a); r.has_c = true; }
        if (std::strcmp(a.d,"N") == 0){ r.n = point(a); r.has_n = true; }
    }

    for (const auto &s : segments){
        for (int seq = s.first; seq <= s.last; ++seq){
            auto it = residues.find({s.chain,seq});
            if (it != residues.end()) it->second.type = s.type;
        }
    }

    modelmesh mesh;
    std::vector<residue> run;
    for (const auto &[key,r] : residues){
        if (!r.has_ca) continue;
        if (!run.empty()){
            const auto &last = run.back();
            const vec delta = r.ca-last.ca;
            if (r.chain != last.chain || r.seq != last.seq+1 || dot(delta,delta) > 25.0f){
                addchain(mesh,run);
                run.clear();
            }
        }
        run.push_back(r);
    }
    addchain(mesh,run);
    return mesh;
}