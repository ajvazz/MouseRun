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
    Player(Genome* genome);

    // Methods used for collision detection and drawing, inherited from QGraphicsItem
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    // Input methods
    void keyPressEvent(QKeyEvent * event) override;
    void keyReleaseEvent(QKeyEvent * event) override;

    // player makes decision based on what neural network from genome tells him
    Genome *genome;
    std::vector<double> genomeInput;
    std::vector<double> genomeOutput;

private:

    // TODO enum
    QMap<char,bool> keysDown;

    qreal speed;    // How fast can the player move
    qreal angle;    // How much SHOULD the player rotate

    qreal energy;     // How many times can the player jump

    QColor color;   // Color of the mouse body
    QColor color2;  // Color of the mouse ears

    bool jumping;   // Is the player jumping
    bool inWater;   // Is the player in water

    static const qreal turningAngle; // How much CAN the player rotate
    static const int maxSpeed;       // Maximum movement speed
    static const int maxEnergy;      // Maximum total energy
    static const qreal consumption;  // How much energy is player losing over time

private slots:
    void move();    // Slot that moves the player
    void update(); // Slot that updates the player

    void moveAllItems(qreal d);
};

#endif // PLAYER_H
