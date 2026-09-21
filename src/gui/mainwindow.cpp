#include <QComboBox>
#include <QLabel>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

mainwindow::mainwindow(QWidget *parent) 
    : QMainWindow(parent), gui(new Ui::MainWindow){
    gui->setupUi(this);

    rendermodec = new QComboBox(this);
    rendermodec->addItem("Structural");
    rendermodec->addItem("Balls");
    rendermodec->addItem("Atoms");
    rendermodec->addItem("Module");

    gui->toolBar->addWidget(new QLabel("View Mode:", this));
    gui->toolBar->addWidget(rendermodec);
    connect(rendermodec, &QComboBox::currentIndexChanged, this, &mainwindow::rendermode);
}

mainwindow::~mainwindow(){
    delete gui;
}

void mainwindow::rendermode(int index){

}