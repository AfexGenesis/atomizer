#include <QApplication>
#include <QVulkanWindow>
#include <QLoggingCategory>
#include <QVulkanInstance>
#include <QFontDatabase>
#include <QFont>
#include <print>
#include "render/window.hpp"
#include "render/vulkan.hpp"
#include "core/cif.hpp"
#include "core/grid.hpp"
#include "core/molecule/module.hpp"
#include "core/molecule/topology.hpp"
#include "gui/mainwindow.hpp"
#include "core/parser.hpp"

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

    moleculedata active;
    if (!cif.empty()){
        active = molecarser(cif);
    }else{
        std::println("no cif");
        // return 69;
    }

    QApplication app(argc, argv);

    QFontDatabase::addApplicationFont(":/resources/fonts/orbitron-regular.ttf");

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

    mainwindow atomizer(active.atoms, active.segments, active.bonds, dbg);
    atomizer.setVulkanInstance(&instance);
    atomizer.resize(800, 800);
    atomizer.show();

    return app.exec();
}