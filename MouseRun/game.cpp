#include "game.h"

#include "cheese.h"
#include "mousetrap.h"
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>

Game::Game()
{
    // Create the scene
    int width = 600;
    int height = 600;

    scene = new QGraphicsScene(this);
    setAlignment(Qt::AlignCenter);

    setFixedSize(width, height);
    setScene(scene);

    // Turn off the scrollbars
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Start the game
    start();
}

void Game::start()
{
    // Spawn player
    player = new Player();
    player->setFlag(QGraphicsItem::ItemIsFocusable);
    player->setFocus();
    player->setZValue(1);
    scene->addItem(player);

    // Update the Game every 15 ms
    static QTimer updateTimer;
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    updateTimer.start(15);

    // Spawn new objects every 500 ms
    static QTimer spawnTimer;
    connect(&spawnTimer, SIGNAL(timeout()), this, SLOT(spawnObjects()));
    spawnTimer.start(1500);

    // Show the scene
    show();
}

void Game::mousePressEvent(QMouseEvent *event)
{

}

void Game::mouseDoubleClickEvent(QMouseEvent *event)
{

}

void Game::update()
{
    if(!player){
        // The player died
        // restart()
        exit(0);
    }

    // Center the view on the player
    centerOn(player);

}

void Game::spawnObjects()
{
    if(QRandomGenerator::global()->bounded(1.0) < 0.3){
        MouseTrap *trap = new MouseTrap();
        trap->setPos(QRandomGenerator::global()->bounded(width()), player->pos().y() - 500);
        trap->setRotation(QRandomGenerator::global()->bounded(1.0));
        scene->addItem(trap);

    }else{
        Cheese *cheese = new Cheese();
        cheese->setPos(QRandomGenerator::global()->bounded(width()), player->pos().y() - 500);
        scene->addItem(cheese);

    }
}
