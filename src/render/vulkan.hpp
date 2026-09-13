#pragma once
#include <QVulkanWindow>
#include <QVulkanWindowRenderer>
#include <DirectXMath.h>
#include <QMutex>
#include <vector>
#include "camera.hpp"
#include "window.hpp"
#include "atoms/atom.hpp"
#include "cif.hpp"

class atomizerer : public QVulkanWindowRenderer {
    public:

    atomizerer(QVulkanWindow *window, const std::vector<atom> &atoms, bool msaa = false);
    //explicit atomizerer(QVulkanWindow *w) : window(w) {}
    void yaw(float degree);
    void pitch(float degree);
    void walk(float amount);
    void strafe(float amount);

    private: 
        void startNextFrame() override;
        void initResources() override;
        void releaseResources() override;
        void initSwapChainResources() override;

        void requestFrame() { windows->requestUpdate(); }
        void getmvp(DirectX::XMFLOAT4X4 *mvp);
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
        uint32_t acount = 0;
        std::vector<atom> atoms;

    DirectX::XMFLOAT4X4 projm;
    camera cam;
    QMutex mutexgui;
};