#pragma once
#include <QVulkanWindow>
#include <QVulkanWindowRenderer>

class atomizerer : public QVulkanWindowRenderer {
    private: QVulkanWindow *atomizing;
    public:
        atomizerer(QVulkanWindow *w) : atomizing(w) {}
        void initResources() override;
        void startNextFrame() override;
};

class atomizer : public QVulkanWindow {
    public: QVulkanWindowRenderer *createRenderer() override;
};