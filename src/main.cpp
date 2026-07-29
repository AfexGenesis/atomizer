#include <QApplication>
#include <QVulkanWindow>
#include <QVulkanWindowRenderer>
#include <QVulkanInstance>
#include <QWidget>
#include <QHBoxLayout>
#include <print>
#include "render/vulkan.hpp"
#include "core/cif.hpp"

int main(int argc, char *argv[]) {

    std::string cif = "";
    for (int input = 1; input < argc; ++input){
        std::string arg = argv[input];
        if ((arg == "input") && (input + 1 < argc)){
            cif = argv[++input];
        }else{
            std::println("type /help for listed commands");
            return 69;
            }
    }

    if (!cif.empty()){
        ciff parser;
        parser.parse(cif);
        std::println("it worked");
    }else{
        std::println("no cif");
        return 69;
    }

    QApplication app(argc, argv);
    
    QVulkanInstance instance;
    if (!instance.create()) return -1;

    QVulkanWindow *vulkanWindow = new QVulkanWindow();
    vulkanWindow->setVulkanInstance(&instance);
    
    QWidget *vulkanWidget = QWidget::createWindowContainer(vulkanWindow);
    QWidget window;
    window.resize(800, 800);
    window.setWindowTitle("atomizer");
    QHBoxLayout *layout = new QHBoxLayout(&window);
    QWidget *sidebar = new QWidget;
    layout->addWidget(vulkanWidget);

    window.show();

    return app.exec();
}