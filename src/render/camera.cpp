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
    float sinyaw, cosyaw, sinpitch, cospitch;
    DirectX::XMScalarSinCos(&sinyaw,&cosyaw,yangle);
    DirectX::XMScalarSinCos(&sinpitch,&cospitch,pangle);
    forward = DirectX::XMFLOAT4(sinyaw * cospitch, sinpitch, cosyaw * cospitch, 0.0f);

    DirectX::XMVECTOR top = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR fvector = DirectX::XMLoadFloat4(&forward);
    DirectX::XMVECTOR svector = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(top, fvector));
    DirectX::XMVECTOR vvector = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(fvector, svector));
    DirectX::XMStoreFloat4(&right, svector);
    DirectX::XMStoreFloat4(&up, vvector);
}

void camera::move(float famount, float samount, float uamount){
    const DirectX::XMVECTOR delta = DirectX::XMLoadFloat4(&forward) * famount + DirectX::XMLoadFloat4(&right)
    * samount + DirectX::XMVectorSet(0.0f, uamount,0.0f, 0.0f);
    DirectX::XMStoreFloat4(&position,DirectX::XMLoadFloat4(&position) + delta);
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

void camera::ray(float x, float y, float aspect, DirectX::XMFLOAT4 &origin, DirectX::XMFLOAT4 &direction) const{
    origin = position;
    DirectX::XMVECTOR raydir = DirectX::XMLoadFloat4(&forward) + DirectX::XMLoadFloat4(&right)
    * (x * aspect) + DirectX::XMLoadFloat4(&up) * y;
    DirectX::XMStoreFloat4(&direction, DirectX::XMVector3Normalize(raydir));
}