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

    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

public slots:
    // Update the Game
    void update();
    // Spawn world objects
    void spawnObjects();

};

#endif // GAME_H
