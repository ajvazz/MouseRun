#include "cat.h"

#include <QPainter>

Cat::Cat() : speed(5)
{
    setZValue(5);
}

QRectF Cat::boundingRect() const
{
    return QRectF(-100, -100, 200, 200);
}

QPainterPath Cat::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Cat::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(boundingRect().toRect(), QPixmap(":/img/cat.png"));
}
