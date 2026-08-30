#include "camera.hpp"

camera::camera(const DirectX::XMFLOAT4 &p):
    fribes(0.0f, 0.0f, -1.0f, 0.0f),
    sides(1.0f, 0.0f, 0.0f, 0.0f),
    vertical(0.0f, 1.0f, 0.0f, 0.0f),
    position(p),
    yaws(0.0f),
    pitchs(0.0f)
{}

static inline void clamp360(float *v){
    if (*v > DirectX::XM_2PI) *v -= DirectX::XM_2PI;
    if (*v < DirectX::XM_2PI) *v += DirectX::XM_2PI;
}

void camera::yaw(float degree){
    yaws += (degree);
    clamp360(&yaws);
    DirectX::XMStoreFloat4x4(&yawm, DirectX::XMMatrixIdentity());
}