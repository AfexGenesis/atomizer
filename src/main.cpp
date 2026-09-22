#include <QApplication>
#include <QVulkanWindow>
#include <QLoggingCategory>
#include <QVulkanInstance>
#include <print>
#include "render/window.hpp"
#include "render/vulkan.hpp"
#include "core/cif.hpp"
#include "core/grid.hpp"
#include "core/molecule/module.hpp"
#include "core/molecule/topology.hpp"
#include "gui/mainwindow.hpp"

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

    grid ag;
    std::vector<segment> segments;
    topology chemistry;
    if (!cif.empty()){
        ciff parser;
        std::vector<atom> a = parser.parse(cif);
        segments = read_secondary(cif);
        chemistry = readtopology(cif, a);
        ag.build(std::move(a));
        std::println("loaded {} atoms and {} bonds", ag.count(), chemistry.bonds.size());
        std::println("bond sources: {} component, {} structure, {} fallback",
                     chemistry.templatecount, chemistry.connectioncount, chemistry.inferredcount);
    }else{
        std::println("no cif");
        // return 69;
    }

    QApplication app(argc, argv);

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

    mainwindow rendervulkan;
    rendervulkan.setVulkanInstance(&instance);
    rendervulkan.resize(800, 800);
    rendervulkan.show();

    return app.exec();
}