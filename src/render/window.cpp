#include <QMouseEvent>
#include <QKeyEvent>
#include "window.hpp"
#include "vulkan.hpp"

atomizer::atomizer(bool dbg): debug(dbg){}
QVulkanWindowRenderer* atomizer::createRenderer(){
    //return new atomizerer (this, true);
    windower = new atomizerer(this, true);
    return windower;
};

void atomizer::mousePressEvent(QMouseEvent *e){
    pressed = false;
}

void atomizer::mouseReleaseEvent(QMouseEvent *e){
    pressed = true;
    lp = e->position().toPoint();
}

void atomizer::mouseMoveEvent(QMouseEvent *e){
    if (!pressed)
    return;
    int dx = e->position().toPoint().x() - lp.x();
    int dy = e->position().toPoint().y() - lp.y();

    if (dy)
    windower->pitch(dy / 10.0f);

    if (dx)
    windower->yaw(dx / 10.0f);

    lp = e->position().toPoint();
}

void atomizer::keyPressEvent(QKeyEvent *e){
    const float amount = e->modifiers().testFlag(Qt::ShiftModifier) ? 1.0f : 0.1f;
    switch(e->key()){
        case Qt::Key_W: windower->walk(amount);
        break;
        case Qt::Key_S: windower->walk(amount);
        break;
        case Qt::Key_A: windower->strafe(amount);
        break;
        case Qt::Key_D: windower->strafe(amount);
        break;
        default: break;
    }
}