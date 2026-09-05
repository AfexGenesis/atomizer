#include <QGuiApplication>
#include <QVulkanWindow>
#include <QLoggingCategory>
#include <QVulkanInstance>
#include <print>
#include "render/window.hpp"
#include "render/vulkan.hpp"
#include "core/cif.hpp"

int main(int argc, char *argv[]) {

    std::string cif = "";
    for (int input = 1; input < argc; ++input){
        std::string arg = argv[input];
        if ((arg == "input") && (input + 1 < argc)){
            cif = argv[++input];
        }else if ((arg == "help")){
            std::println("type ./atomizer input ~protein.cif~");
            //return 69;
        }else{
            std::println("type ./atomizer help for listed commands");
            //return 69;
            }
    }

    if (!cif.empty()){
        ciff parser;
        parser.parse(cif);
        std::println("it worked");
    }else{
        std::println("no cif");
        // return 69;
    }

    QGuiApplication app(argc, argv);

    const bool dbg = qEnvironmentVariableIntValue("QT_VK_DEBUG");
    QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));
    QVulkanInstance instance;
    if (dbg){
        QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));
        instance.setLayers({"VK_LAYER_KHRONOS_validation"});
    }
    if (!instance.create()) {
        throw std::runtime_error("RUN TIME ERROR ON INSTANCE");
    }

    atomizer *vw = new atomizer(dbg);
    vw->setVulkanInstance(&instance);
    
    atomizer window(vw);
    window.setVulkanInstance(&instance);
    window.resize(800, 800);
    window.show();

    return app.exec();
}