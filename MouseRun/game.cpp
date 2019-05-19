#include "game.h"

#include "cheese.h"
#include "mousetrap.h"
#include "waterpool.h"
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>

const int areaH = 400;

Game::Game(std::vector<Genome*> genomes, unsigned bId)
    : bestI{0},
      bId{bId},
      genomes{genomes}
{
    numOfAlive = genomes.size();


    // initialize players
    for(size_t i = 0; i < genomes.size(); i++){
        Player* player = new Player();
        mice.push_back(player);
    }

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

    // Start the game
    start();
}

void Game::start()
{
    // Spawn players
    for(size_t i = 0; i < mice.size(); i++){
        mice[i]->setPos(QRandomGenerator::global()->bounded(-200,200)
                        , 0);
        scene->addItem(mice[i]);
    }

    // Spawn cat
    cat = new Cat();
    cat->setPos(0, 300);
    scene->addItem(cat);

    // Update the Game
    static QTimer updateTimer;
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    updateTimer.start(15);

    // Setup left and right bound
    boundW = 500;
    leftBound  = new WaterBound(15000, boundW);
    rightBound = new WaterBound(15000, boundW);

    leftBound->setPos(-boundW, 0);
    rightBound->setPos(boundW, 0);

    scene->addItem(leftBound);
    scene->addItem(rightBound);

    // initialize item spawning
    spawnObjectsInArea(2);
    spawnObjectsInArea(3);
    spawnObjectsInArea(4);
    bla = areaH;
    areaNum = 4;
    // Show the scene
    show();
}

void Game::makeDecisions()
{

    // make mice think
    for(size_t i = 0; i < mice.size(); i++){
        if(mice[i] != nullptr && mice[i]->alive){
            // feed forward...
            std::vector<double> inputs;
            inputs.push_back(mice[i]->pos().x());
            inputs.push_back(mice[i]->pos().y());
            inputs.push_back(mice[i]->energy);
            inputs.push_back(mice[i]->angle);

//            qDebug() << scene->items().size();
            // Mouse can only see items 1 area ahead (2 x (2 traps, 2 pools, 8 cheese) = 24 items)
            std::vector<QGraphicsItem*> currentArea;
            std::vector<QGraphicsItem*> nextArea;

            foreach (QGraphicsItem* item, scene->items()) {

                int mouseArea = mice[i]->pos().y() / areaH;
                int itemArea = -item->pos().y() / areaH;

                if(Player* x = dynamic_cast<Player*>(item)){
                    continue;
                }
                if(Cat* x = dynamic_cast<Cat*>(item)){
                    continue;
                }
                if(WaterBound* x = dynamic_cast<WaterBound*>(item)){
                    continue;
                }

                if(mouseArea == itemArea){
                    currentArea.push_back(item);
                }else if(mouseArea + 1 == itemArea){
                    nextArea.push_back(item);
                }
            }

//            qDebug() << currentArea.size() << nextArea.size();

            for(size_t i=0; i<currentArea.size(); i++){
                if(Cheese* cheese = dynamic_cast<Cheese*>(currentArea[i])){
                    inputs.push_back(cheese->pos().x());
                    inputs.push_back(cheese->pos().y());
                    inputs.push_back(100);
                }
            }
            for(size_t i=0; i<currentArea.size(); i++){
                if(WaterPool* pool = dynamic_cast<WaterPool*>(currentArea[i])){
                    inputs.push_back(pool->pos().x());
                    inputs.push_back(pool->pos().y());
                    inputs.push_back(-50);
                }
            }
            for(size_t i=0; i<currentArea.size(); i++){
                if(MouseTrap* trap = dynamic_cast<MouseTrap*>(currentArea[i])){
                    inputs.push_back(trap->pos().x());
                    inputs.push_back(trap->pos().y());
                    inputs.push_back(-500);
                }
            }

            for(size_t i=0; i<nextArea.size(); i++){
                if(Cheese* cheese = dynamic_cast<Cheese*>(nextArea[i])){
                    inputs.push_back(cheese->pos().x());
                    inputs.push_back(cheese->pos().y());
                    inputs.push_back(100);
                }
            }
            for(size_t i=0; i<nextArea.size(); i++){
                if(WaterPool* pool = dynamic_cast<WaterPool*>(nextArea[i])){
                    inputs.push_back(pool->pos().x());
                    inputs.push_back(pool->pos().y());
                    inputs.push_back(-50);
                }
            }
            for(size_t i=0; i<nextArea.size(); i++){
                if(MouseTrap* trap = dynamic_cast<MouseTrap*>(nextArea[i])){
                    inputs.push_back(trap->pos().x());
                    inputs.push_back(trap->pos().y());
                    inputs.push_back(-500);
                }
            }

//            qDebug() << inputs.size();


            std::vector<double> outputs = genomes[i]->feedForward(inputs);
            mice[i]->keysDown['w'] = outputs[0] >= 0.5;
            mice[i]->keysDown['a'] = outputs[1] >= 0.5;
            mice[i]->keysDown['s'] = outputs[2] >= 0.5;
            mice[i]->keysDown['d'] = outputs[3] >= 0.5;
        }
    }
}

void Game::update()
{

//    qDebug() << bestI;

    // check for dead mice, determine best mouse
    for(size_t i = 0; i < mice.size(); i++){
        if(!mice[i]){
            continue;
        }
        else if (!mice[i]->alive){
            // A player died
            numOfAlive--;
//            qDebug() << "time" << time.elapsed();
            emit died(bId + i, mice[i]->score);
            delete mice[i];
            mice[i] = nullptr;
        }else{
            if(!mice[bestI] || mice[i]->score > mice[bestI]->score){
                bestI = i;
            }
        }
    }

    if(numOfAlive == 0){
        deleteLater();
        return;
    }

    if(!mice[bestI]){
        for(size_t i = 0; i < mice.size(); i++){
            if(mice[i]){
                bestI = i;
                break;
            }
        }
    }

    // Move the cat == move everything else
    foreach (QGraphicsItem* item, scene->items()) {
        if(Cat *cat = dynamic_cast<Cat*>(item)) continue;
        item->setPos(item->pos().x(), item->pos().y() + cat->speed);
    }

    // Water bound is moving with player
    // If you don't know why 200, enter scene height (=600) or similar, see what happens
    if (leftBound->pos().y() > 200) {
        leftBound->setPos(leftBound->pos().x(), 0);
        rightBound->setPos(rightBound->pos().x(), 0);
    }
    // If mouse mouse goes backward, move water bound below
    if (leftBound->pos().y() < -200) {
        leftBound->setPos(leftBound->pos().x(), 0);
        rightBound->setPos(rightBound->pos().x(), 0);
    }

    // run game normaly
    focusBest();
    spawnObjects();
    deleteObjects();
    makeDecisions();
}

void Game::focusBest(){

    setSceneRect(mice[bestI]->sceneBoundingRect());
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
                     - area * areaH + QRandomGenerator::global()->bounded(0, areaH));

        scene->addItem(item);
    }

    QGraphicsLineItem* line = new QGraphicsLineItem(leftBound->pos().x() + boundW/2,
                                                    - area * areaH,
                                                    rightBound->pos().x() - boundW/2,
                                                    - area * areaH);
    scene->addItem(line);
}

void Game::spawnObjects()
{

    bla -= cat->speed;


    double topY = 0;
    for(size_t i=0; i<mice.size(); i++){
        if(mice[i] && mice[i]->alive && mice[i]->pos().y() < topY){
            topY = mice[i]->pos().y();
        }
    }

    if(bla <= 0){
        if(topY < (areaNum - 3) * (-areaH)){
            spawnObjectsInArea(areaNum++);
        }

        spawnObjectsInArea(areaNum);
        bla = areaH;
    }
}

void Game::deleteObjects()
{
    foreach (QGraphicsItem* item, scene->items()) {
        if (Player *player = dynamic_cast<Player*>(item)){
            if(player->pos().y() > cat->pos().y()){
                player->alive = false;
                continue;
            }
        }
        if (item->pos().y() > cat->pos().y() + 350){
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
