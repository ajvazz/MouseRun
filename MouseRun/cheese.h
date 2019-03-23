#ifndef CHEESE_H
#define CHEESE_H

#include <QGraphicsItem>

class Cheese : public QObject, public QGraphicsItem
{
public:
    Cheese();

//  Methods used for collision detection and drawing, inherited from QGraphicsItem
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    qreal nutrition; // How much energy does a mouse get from eating

private:
    const static int maxNutrition;
};

#endif // CHEESE_H
