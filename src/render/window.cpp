#include <QMouseEvent>
#include <QKeyEvent>
#include "window.hpp"
#include "vulkan.hpp"

atomizer::atomizer(bool dbg): debug(dbg){}
atomizer::atomizer(const std::vector<atom> &atomsis, bool dbg): debug(dbg), atoms(atomsis){}

QVulkanWindowRenderer* atomizer::createRenderer(){
    windower = new atomizerer(this, atoms);
    return windower;
};

void atomizer::mousePressEvent(QMouseEvent *e){
    pressed = false;
    lp = e->position().toPoint();
}

void atomizer::mouseReleaseEvent(QMouseEvent *e){
    pressed = true;
    lp = e->position().toPoint();
}

void atomizer::mouseMoveEvent(QMouseEvent *e){
    if (pressed)
    return;
    int dx = e->position().toPoint().x() - lp.x();
    int dy = e->position().toPoint().y() - lp.y();

    if (dy)
    windower->pitch(dy / 169.420f);

    if (dx)
    windower->yaw(dx / 169.420f);

    lp = e->position().toPoint();
}

void atomizer::keyPressEvent(QKeyEvent *e){
    const float amount = e->modifiers().testFlag(Qt::ShiftModifier) ? 1.0f : 0.1f;
    switch(e->key()){
        case Qt::Key_W: windower->walk(-amount);
        break;
        case Qt::Key_S: windower->walk(amount);
        break;
        case Qt::Key_A: windower->strafe(-amount);
        break;
        case Qt::Key_D: windower->strafe(amount);
        break;
        default: break;
    }
}