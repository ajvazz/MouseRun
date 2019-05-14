#include "cheese.h"
#include <QPainter>
#include <QRandomGenerator>

const int Cheese::maxNutrition = 3;

Cheese::Cheese()
    : nutrition(1 + QRandomGenerator::global()->bounded(maxNutrition))
{
    setZValue(1);
}

QRectF Cheese::boundingRect() const
{
    return QRectF(-13, -13, 26, 26);
}

QPainterPath Cheese::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Cheese::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(boundingRect().toRect(), QPixmap(":/img/cheese.png"));
}
