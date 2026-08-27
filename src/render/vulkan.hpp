#pragma once
#include <QVulkanWindow>
#include <QVulkanWindowRenderer>

class atomizerer : public QVulkanWindowRenderer {
    public:

    atomizerer(QVulkanWindow *window, bool msaa = false);
    //explicit atomizerer(QVulkanWindow *w) : window(w) {}

    private: 
        void startNextFrame() override;
        void initResources() override;
        void releaseResources() override;
        void cleanup();
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
        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        VkPipelineLayout pipeout = VK_NULL_HANDLE;
        VkPipelineCache pipeche = VK_NULL_HANDLE;
        VkPipeline pipelane = VK_NULL_HANDLE;
};  

class atomizer : public QVulkanWindow{
    public: 
    QVulkanWindowRenderer *createRenderer() override;
};