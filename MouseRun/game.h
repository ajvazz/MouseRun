#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>

#include "player.h"
#include "cat.h"

class Game : public QGraphicsView
{
    Q_OBJECT

public:
    Game(std::vector<Genome*> genomes, unsigned bId);

signals:
    void died(size_t i, double score);

public slots:
    // Update the Game
    void update();

private:

    size_t bestI;

    int i;
    unsigned bId;
    QGraphicsScene* scene;
    std::vector<Player*> mice;
    std::vector<Genome*> genomes;
    Cat* cat;

    // Method that initializes the game
    void start();

    // Method that manages object spawning and removal
    void spawnObjectsInArea(int area);
    void spawnObjects();
    void deleteObjects();

    QGraphicsItem *leftBound;
    QGraphicsItem *rightBound;

    double bla;
    int areaNum;

    qreal boundW;
    int numOfAlive;

    void makeDecisions();
    void focusBest();
    void drawGenome(Genome* gen);

    QGraphicsView* nnView;
};

#endif // GAME_H
