
#include "atom.hpp"

atomized::atomized(float radius, uint32_t stacks, uint32_t slices, const DirectX::XMFLOAT4 &col, bool rgb){
    generation(radius, stacks, slices, col, rgb);
}

void atomized::generation(float radius, uint32_t stacks, uint32_t slices, const DirectX::XMFLOAT4 &col, bool rgb){
    vertices.clear();
    indices.clear();

    for (uint32_t i = 0; i <= stacks; ++i){
        float phi = DirectX::XM_PI * (float(i) / float(stacks));
        float y = cosf(phi);
        float rr = sinf(phi);

        for (uint32_t j = 0; j <=slices; ++j){
            float theta = DirectX::XM_2PI * (float(j) / float(slices));
            float x = rr * cosf(theta);
            float z = rr * sinf(theta);

            atomertex v;
            v.position = DirectX::XMFLOAT4(x * radius, y * radius, z * radius, 1.0f);
            v.normal = DirectX::XMFLOAT4(x,y,z,0.0f);
            if(rgb){
                v.colour = DirectX::XMFLOAT4(x * 0.5f + 0.5f, y * 0.5f + 0.5f, z * 0.5f + 0.5f, 1.0f);
            }else{
                v.colour = col;
            }
            vertices.push_back(v);
        }
    }

    for (uint32_t i = 0; i < stacks; ++i){
        for (uint32_t j = 0; j < slices; ++j){
            uint32_t row = slices + 1;
            uint32_t tl = i * row + j;
            uint32_t tr = tl + 1;
            uint32_t bl = (i + 1) * row + j;
            uint32_t br = bl + 1;

            indices.push_back(tl);
            indices.push_back(bl);
            indices.push_back(tr);
            indices.push_back(tr);
            indices.push_back(bl);
            indices.push_back(br);
        }
    }
}