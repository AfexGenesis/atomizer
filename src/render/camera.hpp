#include <DirectXMath.h>

class camera {
    public:
    camera(const DirectX::XMFLOAT4 &p);

    void yaw(float degree);
    void pitch(float degree);
    void walk(float amount);
    void strafe(float amount);
    DirectX::XMFLOAT4X4 matrix() const;

    private:
    DirectX::XMFLOAT4 fribes;
    DirectX::XMFLOAT4 sides;
    DirectX::XMFLOAT4 vertical;
    DirectX::XMFLOAT4 position;
    float yaws;
    float pitchs;

    DirectX::XMFLOAT4X4 yawm;
    DirectX::XMFLOAT4X4 pitchm;
};