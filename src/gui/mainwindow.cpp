#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QFont>
#include <QFontDatabase>
#include <QKeyEvent>
#include <QCoreApplication>
#include "gui/mainwindow.hpp"
#include "ui_mainwindow.h"
#include "render/window.hpp"

mainwindow::mainwindow(const std::vector<atom> &atoms, const std::vector<segment> &segments,
             const std::vector<bond> &bonds, bool dbg, QWidget *parent) 
    : QMainWindow(parent), gui(new Ui::MainWindow){
    gui->setupUi(this);
    connect(gui->actionScreenshot, &QAction::triggered, this, &mainwindow::screenshot);
    QFont orbitron("Orbitron", 10, QFont::Normal);

    rendervulkan = new atomizer(atoms, segments,bonds, dbg);
    vulkanwidget = QWidget::createWindowContainer(rendervulkan, this);

    vulkanwidget->setFocusPolicy(Qt::StrongFocus);
    vulkanwidget->installEventFilter(this);
    rendermodec = new QComboBox(this);
    rendermodec->setFocusPolicy(Qt::NoFocus);
    rendermodec->addItem(" Structural ");
    rendermodec->addItem(" Balls ");
    rendermodec->addItem(" Atoms ");
    rendermodec->addItem(" Module ");

    QVBoxLayout *layout = new QVBoxLayout(gui->vulkanbg);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(vulkanwidget);
    vulkanwidget->setFocusPolicy(Qt::StrongFocus);
    vulkanwidget->setFocus();
    gui->toolBar->setFont(orbitron);
    gui->toolBar->addWidget(new QLabel(this));
    gui->toolBar->addWidget(rendermodec);
    connect(rendermodec, &QComboBox::currentIndexChanged, this, &mainwindow::rendermode);
}

bool mainwindow::eventFilter(QObject *arena, QEvent *event)
{
    if (arena == vulkanwidget) {
        if (event->type() == QEvent::KeyPress ||
            event->type() == QEvent::KeyRelease) {

            QCoreApplication::sendEvent(rendervulkan, event);
            return true;
        }
    }

    return QMainWindow::eventFilter(arena, event);
}

mainwindow::~mainwindow(){
    delete gui;
}

void mainwindow::rendermode(int index){
    const int key = Qt::Key_1 + index;
    QKeyEvent press(QEvent::KeyPress, key, Qt::NoModifier);
    QKeyEvent release(QEvent::KeyRelease, key, Qt::NoModifier);
    QCoreApplication::sendEvent(rendervulkan, &press);
    QCoreApplication::sendEvent(rendervulkan, &release);

    vulkanwidget->setFocus(Qt::MouseFocusReason);
}

void mainwindow::setVulkanInstance(QVulkanInstance *instance) {
    if (rendervulkan) {
        rendervulkan->setVulkanInstance(instance);
    }
}