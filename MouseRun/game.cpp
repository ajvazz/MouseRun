#include "game.h"

#include "cheese.h"
#include "mousetrap.h"
#include "waterpool.h"
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>


Game::Game(std::vector<Genome*> genomes, int bId)
    : bId{bId},
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
    int height = 600;

    scene = new QGraphicsScene(this);
    setAlignment(Qt::AlignCenter);

    setFixedSize(width, height);
    scene->setSceneRect(-300,-300,600,600);
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
    cat->setPos(0, 270);
    scene->addItem(cat);

    // Update the Game every 15 ms
    static QTimer updateTimer;
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    updateTimer.start(30);

    // Setup left and right bound
    boundW = 500;
    leftBound  = new WaterBound(1500, boundW);
    rightBound = new WaterBound(1500, boundW);

    leftBound->setPos(-boundW, 0);
    rightBound->setPos(boundW, 0);

    scene->addItem(leftBound);
    scene->addItem(rightBound);

    // Show the scene and start the time
    time.start();
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

            foreach (QGraphicsItem* item, scene->items()) {
                double value;
                if(Cat *cat = dynamic_cast<Cat*>(item)) {
//                    value = -100;
                    continue;
                } else if(Cheese *cheese = dynamic_cast<Cheese*>(item)) {
                    value = cheese->nutrition;
                } else if(MouseTrap *trap = dynamic_cast<MouseTrap*>(item)) {
                    value = -100;
                } else if(WaterPool *pool = dynamic_cast<WaterPool*>(item)) {
                    value = -10;
                } else if(WaterBound *bound = dynamic_cast<WaterBound*>(item)) {
                    continue;
                } else if(Player *player = dynamic_cast<Player*>(item)) {
                    continue;
                }

                inputs.push_back(item->pos().x());
                inputs.push_back(item->pos().y());
                inputs.push_back(value);
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
    // check for dead mice
    for(size_t i = 0; i < mice.size(); i++){
        if(!mice[i]){
            continue;
        }
        else if (!mice[i]->alive){
            // A player died
            numOfAlive--;
//            qDebug() << "time" << time.elapsed();
            emit died(bId + i, time.elapsed());
            delete mice[i];
            mice[i] = nullptr;
        }
    }

    if(numOfAlive == 0){
        deleteLater();
        return;
    }

    // run game normaly

    spawnObjects();
    deleteObjects();

    makeDecisions();

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
}


void Game::spawnObjects()
{
    while(scene->items().length() < 25 + numOfAlive) {

        qreal p = QRandomGenerator::global()->bounded(1.0);

        QGraphicsItem *item;

        if (p < 0.2){
            item = new MouseTrap();

        } else if (p < 0.8){
            item = new Cheese();

        } else{
            item = new WaterPool(QRandomGenerator::global()->bounded(30, 100),
                                 QRandomGenerator::global()->bounded(30, 130));
        }

        item->setRotation(QRandomGenerator::global()->bounded(-180, 180));
        item->setPos(QRandomGenerator::global()->bounded(int(leftBound->pos().x() + boundW/2),
                                                         int(rightBound->pos().x() - boundW/2)),
                     cat->pos().y() - QRandomGenerator::global()->bounded(600, 1200));

        scene->addItem(item);
    }
}

void Game::deleteObjects()
{
    foreach (QGraphicsItem* item, scene->items()) {
        if (item->pos().y() > cat->pos().y()){
            if (Cheese *cheese = dynamic_cast<Cheese*>(item)){
                cheese->deleteLater();
            }
            else if (MouseTrap *trap = dynamic_cast<MouseTrap*>(item)){
                trap->deleteLater();
            }
            else if (WaterPool *pool = dynamic_cast<WaterPool*>(item)){
                pool->deleteLater();
            }
            else if (Player *player = dynamic_cast<Player*>(item)){
                player->alive = false;
            }
        }
    }
}
