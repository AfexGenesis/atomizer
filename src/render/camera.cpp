#include <algorithm>
#include <cmath>
#include "render/camera.hpp"

camera::camera(const DirectX::XMFLOAT4 &p):
    forward(0.0f, 0.0f, 1.0f, 0.0f),
    right(1.0f, 0.0f, 0.0f, 0.0f),
    up(0.0f, 1.0f, 0.0f, 0.0f),
    position(p.x, p.y, p.z, 1.0f),
    yangle(0.0f),
    pangle(0.0f)
{
    updateBasis();
}

void camera::setPosition(const DirectX::XMFLOAT4 &p){
    position = DirectX::XMFLOAT4(p.x, p.y, p.z, 1.0f);
}

void camera::look(float ydelta, float pdelta){
    yangle = std::remainder(yangle + ydelta, DirectX::XM_2PI);
    constexpr float plimit = DirectX::XM_PIDIV2 - 0.01f;
    pangle = std::clamp(pangle + pdelta, -plimit, plimit);
    updateBasis();
}

void camera::updateBasis(){
    const float cosp = std::cos(pangle);
    forward = DirectX::XMFLOAT4(
        std::sin(yangle) * cosp,
        std::sin(pangle),
        std::cos(yangle) * cosp,
        0.0f
    );

    DirectX::XMVECTOR top = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR fvector = DirectX::XMLoadFloat4(&forward);
    DirectX::XMVECTOR svector = DirectX::XMVector3Normalize(
        DirectX::XMVector3Cross(top, fvector)
    );
    DirectX::XMVECTOR vvector = DirectX::XMVector3Normalize(
        DirectX::XMVector3Cross(fvector, svector)
    );
    DirectX::XMStoreFloat4(&right, svector);
    DirectX::XMStoreFloat4(&up, vvector);
}

void camera::move(float famount, float samount, float uamount){
    position.x += famount * forward.x + samount * right.x;
    position.y += famount * forward.y + samount * right.y + uamount;
    position.z += famount * forward.z + samount * right.z;
}

DirectX::XMFLOAT4X4 camera::matrix() const{
    DirectX::XMMATRIX view = DirectX::XMMatrixLookToLH(
        DirectX::XMLoadFloat4(&position),
        DirectX::XMLoadFloat4(&forward),
        DirectX::XMLoadFloat4(&up)
    );

    DirectX::XMFLOAT4X4 result;
    DirectX::XMStoreFloat4x4(&result, view);
    return result;
}

void camera::ray(float x, float y, float aspect, float origin[3], float direction[3]) const{
    origin[0] = position.x;
    origin[1] = position.y;
    origin[2] = position.z;
    const float dx = forward.x + right.x*x*aspect + up.x*y;
    const float dy = forward.y + right.y*x*aspect + up.y*y;
    const float dz = forward.z + right.z*x*aspect + up.z*y;
    const float length = std::sqrt(dx*dx + dy*dy + dz*dz);
    direction[0] = dx/length;
    direction[1] = dy/length;
    direction[2] = dz/length;
}