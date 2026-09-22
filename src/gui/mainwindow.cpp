#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>

#include "gui/mainwindow.hpp"
#include "ui_mainwindow.h"
#include "render/window.hpp"

mainwindow::mainwindow(QWidget *parent) 
    : QMainWindow(parent), gui(new Ui::MainWindow){
    gui->setupUi(this);

    rendervulkan = new atomizer(true);
    QWidget *vulkanwidget = QWidget::createWindowContainer(rendervulkan, this);
    rendermodec = new QComboBox(this);
    rendermodec->addItem("Structural");
    rendermodec->addItem("Balls");
    rendermodec->addItem("Atoms");
    rendermodec->addItem("Module");

    QVBoxLayout *layout = new QVBoxLayout(gui->vulkanbg);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(vulkanwidget);
    gui->toolBar->addWidget(new QLabel("View Mode:", this));
    gui->toolBar->addWidget(rendermodec);
    connect(rendermodec, &QComboBox::currentIndexChanged, this, &mainwindow::rendermode);
}

mainwindow::~mainwindow(){
    delete gui;
}

void mainwindow::rendermode(int index){

}

void mainwindow::setVulkanInstance(QVulkanInstance *instance) {
    if (rendervulkan) {
        rendervulkan->setVulkanInstance(instance);
    }
}