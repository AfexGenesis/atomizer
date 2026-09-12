#pragma once
#include <QVulkanWindow>
#include <vector>
#include "cif.hpp"

class atomizerer;
class atomizer : public QVulkanWindow{
    public: 
    atomizer(bool dbg);
    atomizer(const std::vector<atom> &atoms, bool dbg);
    
    QVulkanWindowRenderer *createRenderer() override;
    bool isDebugEnabled() const {return debug;}
    int instancecount() const;

    private:
    std::vector<atom> atoms;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    
    bool debug;
    atomizerer *windower;
    bool pressed = false;
    QPoint lp;
};