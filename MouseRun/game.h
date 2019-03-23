#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>

#include "player.h"
#include "cat.h"

class Game : public QGraphicsView
{
    Q_OBJECT

public:
    Game(Genome* genome);


public slots:
    // Update the Game
    void update();

private:

    QGraphicsScene* scene;
    Player* player;
    Cat* cat;

    // Method that initializes the game
    void start(Genome* genome);

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
