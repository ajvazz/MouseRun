#ifndef CAT_H
#define CAT_H

#include <QGraphicsItem>

class Cat : public QObject, public QGraphicsItem
{
public:
    Cat();

//  Methods used for collision detection and drawing, inherited from QGraphicsItem
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    qreal speed;
};

#endif // CAT_H
