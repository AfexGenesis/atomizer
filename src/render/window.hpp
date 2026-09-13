#pragma once
#include <QElapsedTimer>
#include <QSet>
#include <QTimer>
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
    void keyReleaseEvent(QKeyEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
    void updateMovement();
    bool isMovementKey(int key) const;
    
    bool debug;
    atomizerer *windower = nullptr;
    bool looking = false;
    QPoint lp;
    QSet<int> pressedkey;
    QTimer movtimer;
    QElapsedTimer movclock;
};