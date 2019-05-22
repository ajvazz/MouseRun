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

    void spawnObjectsInArea(int area);

    QGraphicsItem *leftBound;
    QGraphicsItem *rightBound;

    double bla;
    int areaNum;

    qreal boundW;
    int numOfAlive;

    void drawGenome(Genome* gen);

    QGraphicsView* nnView;
    double distanceBetween(QGraphicsItem *a, QGraphicsItem *b);


    void spawnObjects();
    void deleteObjects();
    void focusBest();
    void makeDecisions();

private slots:
    void update();

};

#endif // GAME_H
