#pragma once 
#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <render/window.hpp>

namespace Ui{
    class MainWindow;
}
class QComboBox;
class QVulkanWindow;
class QVulkanInstance;
class atomizer;
class QVBoxLayout;

class mainwindow : public QMainWindow{
    Q_OBJECT
    public:
        explicit mainwindow(const std::vector<atom> &atoms, const std::vector<segment> &segments,
             const std::vector<bond> &bonds, bool dbg, QWidget *parent = nullptr);
        ~mainwindow();
        void setVulkanInstance(QVulkanInstance *instance);

    private slots:
        void rendermode (int index);
        void openfile();

    protected:
        bool eventFilter(QObject *arena, QEvent *event) override;

    private:
        Ui::MainWindow * gui;
        QComboBox *rendermodec{nullptr};
        atomizer *rendervulkan{nullptr};
        QWidget *vulkanwidget{nullptr};
};