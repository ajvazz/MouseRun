#include "cheese.h"
#include <QPainter>
#include <QRandomGenerator>

const int Cheese::maxNutrition = 3;

Cheese::Cheese(): nutrition(1 + QRandomGenerator::global()->bounded(maxNutrition)) {}

QRectF Cheese::boundingRect() const
{
    return QRectF(-7, -7, 14, 14);
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
    painter->setBrush(Qt::yellow);
    painter->drawRect(boundingRect());

}
