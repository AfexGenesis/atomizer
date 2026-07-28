#pragma once
#include <QVulkanWindow>
#include <QVulkanWindowRenderer>

class atomizerer : public QVulkanWindowRenderer {
    public:
    atomizerer(QVulkanWindow *w);
    virtual ~atomizerer();
    // void run();

    private: 
        QVulkanWindow *atomizing;
        VkInstance instance;
        void initVulkan();
        void vulkanInstance();
        void startNextFrame() override;
        void initResources() override;
        void cleanup();
        
};

class atomizer : public QVulkanWindow {
    public: QVulkanWindowRenderer *createRenderer() override;
};