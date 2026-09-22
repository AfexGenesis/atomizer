#pragma once
#include <QVulkanWindow>
#include <QVulkanWindowRenderer>
#include <DirectXMath.h>
#include <QMutex>
#include <vector>
#include "render/camera.hpp"
#include "render/window.hpp"
#include "atoms/atom.hpp"
#include "core/cif.hpp"
#include "core/molecule/module.hpp"
#include "render/model/model.hpp"
#include "core/molecule/topology.hpp"

class atomizerer : public QVulkanWindowRenderer {
    public:

    atomizerer(QVulkanWindow *window, const std::vector<atom> &atoms,
               const std::vector<segment> &segments, const std::vector<bond> &bonds,
               bool msaa = false);
    //explicit atomizerer(QVulkanWindow *w) : window(w) {}
    void look(float ydelta, float pdelta);
    void move(float famount, float samount, float vamount, float seconds, bool fast);
    void setMode(int value);
    void pick(int x, int y, int width, int height);

    private: 
        void startNextFrame() override;
        void initResources() override;
        void releaseResources() override;
        void initSwapChainResources() override;

        void requestFrame() { windows->requestUpdate(); }
        struct renderuniforms {
            DirectX::XMFLOAT4X4 view;
            DirectX::XMFLOAT4X4 projection;
        };

        void getUniforms(renderuniforms *uniforms);
        void createHostBuffer(VkBuffer &target, VkDeviceMemory &memory, VkBufferUsageFlags usage, const void *data, VkDeviceSize bytes);
        QVulkanWindow *windows;
        QVulkanDeviceFunctions *devicef;

    protected:
        VkShaderModule createShader(const QString &sildursshader);
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory bufferm = VK_NULL_HANDLE;
        VkDescriptorBufferInfo uallosaursi[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
        VkDescriptorPool pooler = VK_NULL_HANDLE;
        VkDescriptorSetLayout layer = VK_NULL_HANDLE;
        VkDescriptorSet layers[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
        VkPipelineLayout pipeout = VK_NULL_HANDLE;
        VkPipelineCache pipeche = VK_NULL_HANDLE;
        VkPipeline pipelane = VK_NULL_HANDLE;
        uint32_t indexc = 0;
        VkBuffer ibuffer = VK_NULL_HANDLE;
        VkDeviceMemory ibufferm = VK_NULL_HANDLE;
        VkBuffer atomb = VK_NULL_HANDLE;
        VkDeviceMemory atomdm = VK_NULL_HANDLE;
        VkBuffer overlayb = VK_NULL_HANDLE;
        VkDeviceMemory overlaym = VK_NULL_HANDLE;
        VkDeviceSize overlaystride = 0;
        uint32_t acount = 0;
        uint32_t bcount = 0;
        int mode = 1;
        std::vector<atom> atoms;
        std::vector<bond> bonds;
        modelmesh model;
        std::vector<insdata> overlay;
        int selected_piece = -1;
        float movspeed = 5.0f;
        float farplane = 100.0f;
        VkPipeline modelpipe = VK_NULL_HANDLE;
        VkBuffer modelv = VK_NULL_HANDLE;
        VkDeviceMemory modelvm = VK_NULL_HANDLE;
        VkBuffer modeli = VK_NULL_HANDLE;
        VkDeviceMemory modelim = VK_NULL_HANDLE;
        uint32_t modelcount = 0;

    DirectX::XMFLOAT4X4 projm;
    camera cam;
    QMutex mutexgui;
};