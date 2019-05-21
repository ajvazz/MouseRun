#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QGraphicsItem>
#include <vector>

#include "genome.h"

class Player : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    Player();

    // Methods used for collision detection and drawing, inherited from QGraphicsItem
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    // Input methods
    void keyPressEvent(QKeyEvent * event) override;
    void keyReleaseEvent(QKeyEvent * event) override;


    qreal angle;    // How much SHOULD the player rotate

    double traveled;
    double rotated;

    qreal speed;    // How fast can the player move

    QMap<char, bool> keysDown;
    bool alive;

    int advanceBonus;
    double calcFitness();

    QRectF fieldOfVisionForward() const;
    QRectF fieldOfVisionLeft() const;
    QRectF fieldOfVisionRight() const;

private:

    bool inWater;   // Is the player in water
    static const qreal turningAngle; // How much CAN the player rotate
    static const int maxSpeed;       // Maximum movement speed
    static const qreal consumption;  // How much energy is player losing over time

    QColor color;   // Color of the mouse body
    QColor color2;  // Color of the mouse ears


private slots:
    void move();    // Slot that moves the player
    void update(); // Slot that updates the player
};

#endif // PLAYER_H
