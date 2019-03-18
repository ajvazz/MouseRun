#include "player.h"
#include "cheese.h"
#include "mousetrap.h"
#include "waterpool.h"
#include "cat.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>

const int Player::maxSpeed = 6;
const int Player::maxEnergy = 10;
const qreal Player::turningAngle = 0.5;
const qreal Player::consumption = 0.01;


Player::Player(int numInputs, int numOutputs)
    : genome(numInputs, numOutputs), fitness(0),
    angle(0),
    energy(5),
    // Pick random colors for ears and body
    color(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256)),
    color2(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256)),
    jumping(false),
    inWater(false)
{
    setZValue(1);

    setPos(0, 0);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    // Move the player every 15 miliseconds
    static QTimer moveTimer;
    connect(&moveTimer, SIGNAL(timeout()), this, SLOT(move()));
    moveTimer.start(15);

    // Update the player every 50 miliseconds
    static QTimer updateTimer;
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    updateTimer.start(15);

}

// Taken from the CollidingMice example
QRectF Player::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-18 - adjust, -22 - adjust,
                  36 + adjust, 60 + adjust);
}

// Taken from the CollidingMice example
QPainterPath Player::shape() const
{
    QPainterPath path;
    path.addRect(-10, -20, 20, 40);
    return path;
}

// Taken from the CollidingMice example and slightly modified
void Player::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);

    // Body
    painter->setBrush(color);
    painter->drawEllipse(-10, -20, 20, 40);

    // Eyes
    painter->setBrush(Qt::white);
    painter->drawEllipse(-10, -17, 8, 8);
    painter->drawEllipse(2, -17, 8, 8);

    // Nose
    painter->setBrush(Qt::black);
    painter->drawEllipse(QRectF(-2, -22, 4, 4));

    // Pupils
    painter->drawEllipse(QRectF(-8.0, -17, 4, 4));
    painter->drawEllipse(QRectF(4.0, -17, 4, 4));

    // Ears
    painter->setBrush(color2);
    painter->drawEllipse(-17, -12, 16, 16);
    painter->drawEllipse(1, -12, 16, 16);

    // Tail
    QPainterPath path(QPointF(0, 20));
    path.cubicTo(-5, 22, -5, 22, 0, 25);
    path.cubicTo(5, 27, 5, 32, 0, 30);
    path.cubicTo(-5, 32, -5, 42, 0, 35);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);
}

void Player::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_A || event->key() == Qt::Key_Left){
        keysDown['a'] = true;
    }
    if(event->key() == Qt::Key_D || event->key() == Qt::Key_Right){
        keysDown['d'] = true;
    }
    if(event->key() == Qt::Key_W || event->key() == Qt::Key_Up){
        keysDown['w'] = true;
    }
    if(event->key() == Qt::Key_S || event->key() == Qt::Key_Down){
        keysDown['s'] = true;
    }
    if(event->key() == Qt::Key_Space){
        keysDown['j'] = true;
    }
    if(event->key() == Qt::Key_E){
        energy++;
    }
}

void Player::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_A || event->key() == Qt::Key_Left){
        keysDown['a'] = false;
    }
    if(event->key() == Qt::Key_D || event->key() == Qt::Key_Right){
        keysDown['d'] = false;
    }
    if(event->key() == Qt::Key_W || event->key() == Qt::Key_Up){
        keysDown['w'] = false;
    }
    if(event->key() == Qt::Key_S || event->key() == Qt::Key_Down){
        keysDown['s'] = false;
    }
    if(event->key() == Qt::Key_Space){
        keysDown['j'] = false;
    }
}


void Player::move()
{

    // JUMP

    // The jump 'animation' lasts for 15 ms
    static int jumpTime = 0;
    static const int maxJumpTime = 15;

    // Start/continue the animation if there is enough energy
    if(((keysDown['j'] && jumpTime == 0) || jumping) && energy > 0 && (!inWater || jumping)){

        if(!jumping){
            jumping = true; // Start
        }

        // 'Animation'
        if(jumpTime <= 4){
            setScale(1.0 + qreal(jumpTime)/maxJumpTime);
        }else if(jumpTime >= maxJumpTime - 4){
            setScale(1.5 - qreal(jumpTime)/(maxJumpTime*2));
        }else{
            setScale(1.5);
        }

        // Jump moves the player forward = moves everything else backward
        moveAllItems(maxSpeed);

        jumpTime++; // Time passes...

        // If animation time has passed, end the jump
        if(jumpTime >= maxJumpTime){
            jumping = false;
            energy--; // Jumping costs large amount of energy
            jumpTime = 0;
            setScale(1);
        }
        return;
    }

    // MOVE
    // Move forward = Move everything else backward
    if(keysDown['w'] && !keysDown['s']){
        moveAllItems(speed);
    }
    // Move backward = Move everything else forward
    else if(keysDown['s'] && !keysDown['w']){
        moveAllItems(-speed/2);
    }

    // ROTATE
    // Rotate left
    if(keysDown['a'] && !keysDown['d']){
        angle = -turningAngle;
    }
    // Rotate right
    else if(keysDown['d'] && !keysDown['a']){
        angle = turningAngle;
    }

    // Rotate
    if(angle){
        qreal dx = sin(angle) * 20;
        setRotation(rotation() + dx);
        angle = 0;
    }
}

void Player::update()
{
    // The more energy the player has, the faster he moves
    speed = maxSpeed/3.0 + (energy * (2.0 * maxSpeed / maxEnergy / 3.0));

    // Unless he is trying to move trough the water
    if(inWater) speed = 1;
    inWater = false;

    // Player is losing energy over time
    energy -= consumption;
    if(energy <= 0) energy = 0;
    if(energy >= maxEnergy) energy = maxEnergy;
//    qDebug() << pos() << energy << speed;


    // Check for collision with other items
    QList<QGraphicsItem*> collided = collidingItems();
    foreach(QGraphicsItem* item, collided){

        // If the item is a cheese, eat it
        if(Cheese *cheese = dynamic_cast<Cheese*>(item)){
            energy += cheese->nutrition;
            cheese->deleteLater();
        }
        // If the item is a trap, die
        else if(MouseTrap *trap = dynamic_cast<MouseTrap*>(item)){
            deleteLater();
        }

        else if(WaterPool *pool = dynamic_cast<WaterPool*>(item)){
            inWater = true;
        }
        else if(Cat *cat = dynamic_cast<Cat*>(item)){
            deleteLater();
//            qDebug() << "DEAD";
        }
    }
}

void Player::moveAllItems(qreal d)
{
    foreach(QGraphicsItem* item, scene()->items()){
        if(item != this){
            QPointF newPos = item->pos() + mapToParent(0, d);
            item->setPos(item->pos().x(), newPos.y());
        }else{
            QPointF newPos = mapToParent(0, -d);
            setPos(newPos.x(), pos().y());
        }
    }
}
