#include "mousetrap.h"
#include <QPainter>


MouseTrap::MouseTrap(){
    setZValue(0);
}

QRectF MouseTrap::boundingRect() const
{
    return QRectF(-20, -30, 45, 75);
}

QPainterPath MouseTrap::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void MouseTrap::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(boundingRect().toRect(), QPixmap(":/img/mousetrap.png"));
}
