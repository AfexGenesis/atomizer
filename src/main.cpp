#include <QApplication>
#include <QVulkanWindow>
#include <QVulkanWindowRenderer>
#include <QVulkanInstance>
#include <QWidget>
#include "render/vulkan.hpp"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    
    QWidget window;
    window.resize(800, 800);
    window.setWindowTitle("atomizer");
    window.show();

    return app.exec();
}