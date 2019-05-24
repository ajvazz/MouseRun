#ifndef MOUSETRAP_H
#define MOUSETRAP_H

#include <QObject>
#include <QGraphicsItem>

class MouseTrap : public QObject, public QGraphicsItem
{
public:
    MouseTrap();

//     Methods used for collision detection and drawing, inherited from QGraphicsItem
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;


};

#endif // MOUSETRAP_H
