#pragma once 
#include <QMainWindow>

namespace Ui{
    class MainWindow;
}
class QComboBox;
class QVulkanWindow;
class QVulkanInstance;

class mainwindow : public QMainWindow{
    Q_OBJECT
    public:
        explicit mainwindow(QWidget *parent = nullptr);
        ~mainwindow();
        void setVulkanInstance(QVulkanInstance *instance);

    private slots:
        void rendermode (int index);

    private:
        Ui::MainWindow * gui;
        QComboBox *rendermodec{nullptr};
        QVulkanWindow *rendervulkan{nullptr};
};