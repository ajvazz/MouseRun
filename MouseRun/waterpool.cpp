#include "waterpool.h"

#include <QPainter>

WaterPool::WaterPool(qreal h, qreal w): height(h), width(w)
{
    setZValue(-1);
}

QRectF WaterPool::boundingRect() const
{
    return QRectF(-width/2, -height/2, width, height);
}

QPainterPath WaterPool::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

void WaterPool::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::blue);
    painter->drawEllipse(boundingRect());
}

WaterBound::WaterBound(qreal h, qreal w) : WaterPool (h, w)
{

}

QPainterPath WaterBound::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void WaterBound::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::blue);
    painter->drawRect(boundingRect());
}
