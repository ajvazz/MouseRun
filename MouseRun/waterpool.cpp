#include "waterpool.h"

#include <QPainter>
#include <QMovie>
#include <QLabel>

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

void WaterPool::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QBrush(QPixmap(":/img/water1.png")));
    painter->drawEllipse(boundingRect());
}

WaterBound::WaterBound(qreal h, qreal w): WaterPool (h, w)
{
    setZValue(0);
}

QPainterPath WaterBound::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void WaterBound::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawTiledPixmap(boundingRect().toRect(), QPixmap(":/img/water1.png"), QPoint(0,0));
}
