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


public slots:
    // Update the Game
    void update();
    // Spawn world objects

    void getNodeId(Genome* genome, int connectionId);

    void getConnId(Genome* genome, int fromNodeId, int toNodeId);

private:

    QGraphicsScene* scene;
    Player* player;
    Cat* cat;

    // population of genetic algorithm
    std::vector<Player*> population;

    // current generation
    int generation;

    // Every structural mutation must be saved - innovation numbers (ids)
    // of both nodes and connections must be unique for entire generation

    // maps ids of two nodes' ids to id of connection between them - used for addConnection mutation
    std::map<std::pair<int, int>, int> mapConn;

    // if given key doesn't exist in the mapConn, nextConnId is incremented and set as value in the map
    int nextConnId;

    // maps connection id between two nodes to id of new node between them - used for addNode mutation
    std::map<int, int> mapNode;

    // if given key doesn't exist in the mapNode, nextNodeId is incremented and set as value in the map
    int nextNodeId;


    // Method that initializes the game
    void start();

    // Method that manages object spawning and removal
    void spawnObjects();
    void deleteObjects();

    // Disable player deselection
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    QGraphicsItem *leftBound;
    QGraphicsItem *rightBound;

    qreal boundW;


};

#endif // GAME_H
