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
            std::print("type /help for listed commands");
            }
    }

    if (!cif.empty()){
        ciff parser;
        parser.parse(cif);
        std::print("it worked");
    }else{
        std::print("no cif");
    }

    QApplication app(argc, argv);
    
    QVulkanInstance inst;
    
    atomizer *vulkanWindow = new atomizer();
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