#pragma once
#include <QVulkanWindow>
#include <QVulkanWindowRenderer>

class atomizerer : public QVulkanWindowRenderer {
    public:
    atomizerer(QVulkanWindow *w);

    private: 
        QVulkanInstance instance;
        void startNextFrame() override;
        void initResources() override;
        void cleanup();
        
};

class atomizer : public QVulkanWindow {
    public: QVulkanWindowRenderer *createRenderer() override;
};