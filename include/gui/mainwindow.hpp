#pragma once 
#include <QMainWindow>
#include <QWidget>
#include <render/window.hpp>

namespace Ui{
    class MainWindow;
}
class QComboBox;
class QVulkanWindow;
class QVulkanInstance;
class atomizer;

class mainwindow : public QMainWindow{
    Q_OBJECT
    public:
        explicit mainwindow(const std::vector<atom> &atoms, const std::vector<segment> &segments,
             const std::vector<bond> &bonds, bool dbg, QWidget *parent = nullptr);
        ~mainwindow();
        void setVulkanInstance(QVulkanInstance *instance);

    private slots:
        void rendermode (int index);

    private:
        Ui::MainWindow * gui;
        QComboBox *rendermodec{nullptr};
        QVulkanWindow *rendervulkan{nullptr};
};