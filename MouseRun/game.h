#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>

#include "player.h"

class Game : public QGraphicsView
{
    Q_OBJECT

public:
    Game();

private:

    QGraphicsScene* scene;
    Player* player;

    // Method that initializes the game
    void start();

    // Methot that manages object spawning and removal
    void spawnObjects();
    void deleteObjects();

    // Disable player deselection
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    QGraphicsItem *leftBound;
    QGraphicsItem *rightBound;

    qreal boundW;

public slots:
    // Update the Game
    void update();
    // Spawn world objects

};

#endif // GAME_H
