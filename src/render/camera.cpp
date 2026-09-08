#include "camera.hpp"

camera::camera(const DirectX::XMFLOAT4 &p):
    fribes(0.0f, 0.0f, -1.0f, 0.0f),
    sides(1.0f, 0.0f, 0.0f, 0.0f),
    vertical(0.0f, 1.0f, 0.0f, 0.0f),
    position(p.x, p.y, p.z, 1.0f),
    yaws(0.0f),
    pitchs(0.0f)
{
    DirectX::XMStoreFloat4x4(&yawm, DirectX::XMMatrixIdentity());
    DirectX::XMStoreFloat4x4(&pitchm, DirectX::XMMatrixIdentity());
}

static inline void clamp360(float *v){
    if (*v > DirectX::XM_2PI) *v -= DirectX::XM_2PI;
    if (*v < -DirectX::XM_2PI) *v += DirectX::XM_2PI;
}

void camera::yaw(float degree){
    yaws += (degree);
    clamp360(&yaws);

    DirectX::XMMATRIX pm = DirectX::XMLoadFloat4x4(&pitchm);
    DirectX::XMMATRIX ym = DirectX::XMMatrixRotationY(yaws);
    DirectX::XMStoreFloat4x4(&yawm, ym);
    DirectX::XMMATRIX rm = DirectX::XMMatrixMultiply(pm, ym);

    DirectX::XMVECTOR qfribes = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0);
    DirectX::XMVECTOR qsides = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR wfribes = DirectX::XMVector4Transform(qfribes, rm);
    DirectX::XMVECTOR wsides = DirectX::XMVector4Transform(qsides, rm);

    DirectX::XMStoreFloat4(&fribes, wfribes);
    DirectX::XMStoreFloat4(&sides, wsides);
}

void camera::pitch(float degree){
    pitchs +=(degree);
    clamp360(&pitchs);

    DirectX::XMMATRIX pmm = DirectX::XMLoadFloat4x4(&yawm);
    DirectX::XMMATRIX ymm = DirectX::XMMatrixRotationX(pitchs);
    DirectX::XMStoreFloat4x4(&pitchm, ymm);
    DirectX::XMMATRIX rmm = DirectX::XMMatrixMultiply(pmm, ymm);

    DirectX::XMVECTOR qqfribes = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
    DirectX::XMVECTOR qvertical = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); 
    DirectX::XMVECTOR wwfribes = DirectX::XMVector4Transform(qqfribes, ymm);
    DirectX::XMVECTOR wvertical = DirectX::XMVector4Transform(qvertical, rmm);

    DirectX::XMStoreFloat4(&fribes, wwfribes);
    DirectX::XMStoreFloat4(&sides, wvertical);
}

void camera::walk(float amount){
    position.x += amount * fribes.x;
    position.z += amount * fribes.z;
}

void camera::strafe(float amount){
    position.x += amount * sides.x;
    position.z += amount * sides.z;
}

DirectX::XMFLOAT4X4 camera::matrix() const{
    DirectX::XMMATRIX pmmm = DirectX::XMLoadFloat4x4(&pitchm);
    DirectX::XMMATRIX ymmm = DirectX::XMLoadFloat4x4(&yawm);
    DirectX::XMMATRIX rmmm = DirectX::XMMatrixMultiply(pmmm, ymmm);
    DirectX::XMMATRIX tm = DirectX::XMMatrixTranslation(-position.x, -position.y, -position.z);
    DirectX::XMMATRIX v = DirectX::XMMatrixMultiply(tm, rmmm);

    DirectX::XMFLOAT4X4 result;
    DirectX::XMStoreFloat4x4(&result, v);
    return result;
}