#pragma once
#include <QVulkanWindow>
#include <QVulkanWindowRenderer>

class atomizerer : public QVulkanWindowRenderer {
    public:
    explicit atomizerer(QVulkanWindow *w) : window(w) {}

    private: 
        void startNextFrame() override;
        void initResources() override;
        void cleanup();
        QVulkanWindow *window;
};

class atomizer : public QVulkanWindow{
    public: 
    QVulkanWindowRenderer *createRenderer() override;
};