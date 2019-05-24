#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>

#include "player.h"
#include "cat.h"

class Game : public QGraphicsView
{
    Q_OBJECT

public:
    Game();

private:
    QGraphicsScene* scene;
    Player* player;
    Cat* cat;

    // Method that initializes the game
    void start();

    void spawnObjectsInArea(int area);

    QGraphicsItem *leftBound;
    QGraphicsItem *rightBound;

    double bla;
    int areaNum;

    qreal boundW;
    int numOfAlive;

    double distanceBetween(QGraphicsItem *a, QGraphicsItem *b);


    void spawnObjects();
    void deleteObjects();
    void focusPlayer();

private slots:
    void update();

};

#endif // GAME_H
