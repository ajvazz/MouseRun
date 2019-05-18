#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>
#include <QTime>

#include "player.h"
#include "cat.h"

class Game : public QGraphicsView
{
    Q_OBJECT

public:
    Game(std::vector<Genome*> genomes, int bId);

signals:
    void died(size_t i, double score);

public slots:
    // Update the Game
    void update();

private:
    int i;
    int bId;
    QGraphicsScene* scene;
    std::vector<Player*> mice;
    std::vector<Genome*> genomes;
    Cat* cat;

    // Method that initializes the game
    void start();

    // Method that manages object spawning and removal
    void spawnObjects();
    void deleteObjects();

    QGraphicsItem *leftBound;
    QGraphicsItem *rightBound;

    qreal boundW;

    int numOfAlive;

    QTime time;
    void makeDecisions();
};

#endif // GAME_H
