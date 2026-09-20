#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <algorithm>
#include "window.hpp"
#include "vulkan.hpp"

atomizer::atomizer(bool dbg): debug(dbg){
    movtimer.setInterval(16);
    connect(&movtimer, &QTimer::timeout, this, &atomizer::updateMovement);
}

atomizer::atomizer(const std::vector<atom> &atomsis, const std::vector<segment> &segmentsis,
                   const std::vector<bond> &bondsis, bool dbg): atomizer(dbg){
    atoms = atomsis;
    segments = segmentsis;
    bonds = bondsis;
}

QVulkanWindowRenderer* atomizer::createRenderer(){
    windower = new atomizerer(this, atoms, segments, bonds);
    return windower;
};

void atomizer::mousePressEvent(QMouseEvent *e){
    requestActivate();
    if (e->button() == Qt::RightButton){
        looking = true;
        lp = e->position().toPoint();
        e->accept();
    }else if (e->button() == Qt::LeftButton && windower){
        windower->pick(e->position().toPoint().x(), e->position().toPoint().y(), width(), height());
        e->accept();
    }
}

void atomizer::mouseReleaseEvent(QMouseEvent *e){
    if (e->button() == Qt::RightButton){
        looking = false;
        e->accept();
    }
}

void atomizer::mouseMoveEvent(QMouseEvent *e){
    if (!looking || !windower)
        return;

    const int dx = e->position().toPoint().x() - lp.x();
    const int dy = e->position().toPoint().y() - lp.y();
    if (dx || dy)
    windower->look(dx / 169.420f, -dy / 169.420f);

    lp = e->position().toPoint();
}

void atomizer::keyPressEvent(QKeyEvent *e){
    if (windower && e->key() >= Qt::Key_1 && e->key() <= Qt::Key_4){
        windower->setMode(e->key() - Qt::Key_0);
        e->accept();
        return;
    }
    if (isMovementKey(e->key())){
        if (!e->isAutoRepeat())
            pressedkey.insert(e->key());
        if (!movtimer.isActive()){
            movclock.restart();
            movtimer.start();
        }
        e->accept();
        return;
    }
    QVulkanWindow::keyPressEvent(e);
}

void atomizer::keyReleaseEvent(QKeyEvent *e){
    if (isMovementKey(e->key())){
        if (!e->isAutoRepeat())
            pressedkey.remove(e->key());
        e->accept();
        return;
    }
    QVulkanWindow::keyReleaseEvent(e);
}

void atomizer::focusOutEvent(QFocusEvent *e){
    pressedkey.clear();
    movtimer.stop();
    looking = false;
    QVulkanWindow::focusOutEvent(e);
}

bool atomizer::isMovementKey(int key) const{
    return key == Qt::Key_W || key == Qt::Key_A || key == Qt::Key_S ||
    key == Qt::Key_D || key == Qt::Key_C || key == Qt::Key_V ||
    key == Qt::Key_Shift;
}

void atomizer::updateMovement(){
    if (!windower)
        return;

    const float seconds = std::min(movclock.restart() / 1000.0f, 0.05f);
    const float forwardAmount = float(pressedkey.contains(Qt::Key_W)) - float(pressedkey.contains(Qt::Key_S));
    const float rightAmount = float(pressedkey.contains(Qt::Key_D)) - float(pressedkey.contains(Qt::Key_A));
    const float upAmount = float(pressedkey.contains(Qt::Key_V)) - float(pressedkey.contains(Qt::Key_C));

    if (forwardAmount == 0.0f && rightAmount == 0.0f && upAmount == 0.0f){
        movtimer.stop();
        return;
    }

    windower->move(
        forwardAmount,
        rightAmount,
        upAmount,
        seconds,
        pressedkey.contains(Qt::Key_Shift)
    );
}