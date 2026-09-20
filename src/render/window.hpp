#pragma once
#include <QElapsedTimer>
#include <QSet>
#include <QTimer>
#include <QVulkanWindow>
#include <vector>
#include "cif.hpp"
#include "molecule/module.hpp"
#include "molecule/topology.hpp"

class atomizerer;
class atomizer : public QVulkanWindow{
    public: 
    atomizer(bool dbg);
    atomizer(const std::vector<atom> &atoms, const std::vector<segment> &segments,
             const std::vector<bond> &bonds, bool dbg);
    
    QVulkanWindowRenderer *createRenderer() override;
    bool isDebugEnabled() const {return debug;}
    int instancecount() const;

    private:
    std::vector<atom> atoms;
    std::vector<segment> segments;
    std::vector<bond> bonds;
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