#pragma once
#include <QVulkanWindow>

class atomizerer;
class atomizer : public QVulkanWindow{
    public: 
    atomizer(bool dbg);
    
    QVulkanWindowRenderer *createRenderer() override;
    bool isDebugEnabled() const {return debug;}
    int instancecount() const;

    private:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    
    bool debug;
    atomizerer *windower;
    bool pressed = false;
    QPoint lp;
};