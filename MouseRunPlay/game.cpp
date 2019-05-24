#include "game.h"

#include "cheese.h"
#include "mousetrap.h"
#include "waterpool.h"
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>

const int areaH = 400;

Game::Game()
{
    // initialize player
    player = new Player();


    // Create the scene
    int width = 600;
    int height = 800;

    scene = new QGraphicsScene(this);
    setAlignment(Qt::AlignCenter);

    setFixedSize(width, height);
    scene->setSceneRect(-300, -300, width, height);
    setScene(scene);

    // Turn off the scrollbars
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setBackgroundBrush(QBrush(QColor(55,205,55)));


    move(0,0);
    // Start the game
    start();
}

void Game::start()
{
    // Spawn players
    player->setPos(0, -300);
    scene->addItem(player);

    // Spawn cat
    cat = new Cat();
    cat->setPos(0, 0);
    scene->addItem(cat);

    // Update the Game
    static QTimer updateTimer;
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    updateTimer.start(15);


    // Setup left and right bound
    boundW = 500;
    leftBound  = new WaterBound(1500, boundW);
    rightBound = new WaterBound(1500, boundW);

    leftBound->setPos(-boundW, -300);
    rightBound->setPos(boundW, -300);

    scene->addItem(leftBound);
    scene->addItem(rightBound);


    // initialize item spawning
//    spawnObjectsInArea(1);
    spawnObjectsInArea(2);
    spawnObjectsInArea(3);
    spawnObjectsInArea(4);
    bla = areaH;
    areaNum = 4;

    // Show the scene
    show();
}

double Game::distanceBetween(QGraphicsItem* a, QGraphicsItem* b){

    double dx = a->pos().x() - b->pos().x();
    double dy = a->pos().y() - b->pos().y();

    return sqrt(dx*dx + dy*dy);
}

void Game::update()
{



    if(player->pos().y() >= cat->pos().y()){
        player->alive = false;
    }

    if(!player->alive){
        exit(0);
    }

    // Move the cat == move everything else
    foreach (QGraphicsItem* item, scene->items()) {
        if(Cat *cat = dynamic_cast<Cat*>(item)) continue;
        if(WaterBound *b = dynamic_cast<WaterBound*>(item)) continue;
        item->setPos(item->pos().x(), item->pos().y() + cat->speed);
    }


    // run game normaly
    spawnObjects();
    focusPlayer();
    deleteObjects();
}

void Game::focusPlayer(){

    // sceneWidth = 600, sceneHeight = 800
    setSceneRect(-300, player->y() - 400, 600, 800);
}


void Game::spawnObjectsInArea(int area)
{
    //spawn 2 mousetraps, 2 waterpools and 8 pieces of cheese
    for(int i = 0; i < 12; i++) {

        QGraphicsItem *item;

        if (i < 2){
            item = new MouseTrap();

        } else if (i < 10){
            item = new Cheese();

        } else{
            item = new WaterPool(QRandomGenerator::global()->bounded(30, 100),
                                 QRandomGenerator::global()->bounded(30, 130));
        }

        item->setRotation(QRandomGenerator::global()->bounded(-180, 180));
        item->setPos(QRandomGenerator::global()->bounded(int(leftBound->pos().x() + boundW/2),
                                                         int(rightBound->pos().x() - boundW/2)),
                     - area * areaH + QRandomGenerator::global()->bounded(5, areaH - 5));

        scene->addItem(item);
    }

//    QGraphicsLineItem* line = new QGraphicsLineItem(leftBound->pos().x() + boundW/2,
//                                                    - area * areaH,
//                                                    rightBound->pos().x() - boundW/2,
//                                                    - area * areaH);
//    scene->addItem(line);
}

void Game::spawnObjects()
{

    bla -= cat->speed;


    if(bla <= 0){
        if(player->y() < (areaNum - 3) * (-areaH)){
            spawnObjectsInArea(areaNum++);
        }

        spawnObjectsInArea(areaNum);
        bla = areaH;
    }
}

void Game::deleteObjects()
{

    foreach (QGraphicsItem* item, scene->items()) {
        if (item->pos().y() > cat->pos().y() + 500){
            if (Cheese *cheese = dynamic_cast<Cheese*>(item)){
                cheese->deleteLater();
            }
            else if (MouseTrap *trap = dynamic_cast<MouseTrap*>(item)){
                trap->deleteLater();
            }
            else if (WaterPool *pool = dynamic_cast<WaterPool*>(item)){
                pool->deleteLater();
            }
        }
    }
}
