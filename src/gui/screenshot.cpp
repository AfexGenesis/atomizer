#include <QFileDialog>
#include <QImage>
#include <QMessageBox>
#include <QStandardPaths>
#include "gui/mainwindow.hpp"

void mainwindow::screenshot(){
    if(!rendervulkan || !rendervulkan->supportsGrab()){
        QMessageBox::warning(this, tr("screenshot failed"), tr("failed 69")); 
        return;
    }
    const QString screen = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    const QString filelocation = QFileDialog::getSaveFileName(this, tr("screenshot success"),
    screen + QStringLiteral("/atomizer.png"));
    if (filelocation.isEmpty()){
        QMessageBox::warning(this, tr("file location is unable to locate the location of the located file of the location"), tr("filelocation is empty"));
        return;
    }

    const QImage image = rendervulkan->grab();
    if (image.isNull()){
        QMessageBox::warning(this, tr("screenshot failed"), tr("failed to screenshot :error{null.69}"));
        return;
    }
    if(!image.save(filelocation)){
        QMessageBox::warning(this, tr("screenshot failed"), tr("failed to save :error{filelocation.69}"));
        return; 
    }
}