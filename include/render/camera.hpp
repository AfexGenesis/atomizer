#include <DirectXMath.h>

class camera {
    public:
    camera(const DirectX::XMFLOAT4 &p);

    void setPosition(const DirectX::XMFLOAT4 &p);
    void look(float ydelta, float pdelta);
    void move(float famount, float ramount, float uamount);
    DirectX::XMFLOAT4X4 matrix() const;
    void ray(float x, float y, float aspect, DirectX::XMFLOAT4 &origin, DirectX::XMFLOAT4 &direction) const;

    private:
    void updateBasis();

    DirectX::XMFLOAT4 forward;
    DirectX::XMFLOAT4 right;
    DirectX::XMFLOAT4 up;
    DirectX::XMFLOAT4 position;
    float yangle;
    float pangle;
};