#include "game.h"

#include "cheese.h"
#include "mousetrap.h"
#include "waterpool.h"
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
    scene->setSceneRect(-300,-300,600,600);
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
    scene->addItem(player);

    // Update the Game every 15 ms
    static QTimer updateTimer;
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    updateTimer.start(15);

    // Spawn new objects every 500 ms
    static QTimer spawnTimer;
    connect(&spawnTimer, SIGNAL(timeout()), this, SLOT(spawnObjects()));
    spawnTimer.start(1500);

    // Setup left and right bound
    boundW = 500;
    leftBound = new WaterBound(1000, boundW);
    rightBound = new WaterBound(1000, boundW);
    leftBound->setPos(-boundW, 0);
    rightBound->setPos(boundW, 0);

    scene->addItem(leftBound);
    scene->addItem(rightBound);

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
//    centerOn(player);

    // Reset bounds positions so they appear to be infinite
    leftBound->setPos(leftBound->pos().x(), 0);
    rightBound->setPos(rightBound->pos().x(), 0);

    qDebug() << scene->items().size();

}

void Game::spawnObjects()
{
    qreal p = QRandomGenerator::global()->bounded(1.0);

    QGraphicsItem *item;

    if(p < 0.2){
        item = new MouseTrap();

    }else if (p < 0.8){
        item = new Cheese();

    } else{
        item = new WaterPool(QRandomGenerator::global()->bounded(30, 100), QRandomGenerator::global()->bounded(30, 130));
    }


    item->setRotation(QRandomGenerator::global()->bounded(-180, 180));
    item->setPos(QRandomGenerator::global()->bounded(int(leftBound->pos().x() + boundW/2), int(rightBound->pos().x() - boundW/2)), player->pos().y() - 500);
    scene->addItem(item);
}
