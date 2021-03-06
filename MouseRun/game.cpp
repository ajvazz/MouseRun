﻿#include "game.h"

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


    // Window for nn
    nnView = new QGraphicsView();
    nnView->setScene(new QGraphicsScene(nnView));
    nnView->setRenderHint(QPainter::Antialiasing);
    nnView->move(600, 0);
    move(0,0);
    // Start the game
    start();
}

void Game::start()
{
    // Spawn players
    for(size_t i = 0; i < mice.size(); i++){
        mice[i]->setPos(QRandomGenerator::global()->bounded(-200,200)
                        , -300);
        scene->addItem(mice[i]);

    }

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
    drawGenome(genomes[0]);
    show();
    nnView->show();
}

double Game::distanceBetween(QGraphicsItem* a, QGraphicsItem* b){

    double dx = a->pos().x() - b->pos().x();
    double dy = a->pos().y() - b->pos().y();

    return sqrt(dx*dx + dy*dy);
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

            double rot = std::fmod(mice[i]->rotation(), 360);

            if(rot > 180){
                rot = -( 360 - rot);
            }else if(rot < -180){
                rot = 360 + rot;
            }

            inputs.push_back(rot);


// INPUTS V3
            QList<QGraphicsItem*> visibleFront = scene->items(mice[i]->mapRectToScene(mice[i]->fieldOfVisionForward()),
                                                              Qt::IntersectsItemShape, Qt::AscendingOrder);
            QList<QGraphicsItem*> visibleLeft = scene->items(mice[i]->mapRectToScene(mice[i]->fieldOfVisionLeft()),
                                                             Qt::IntersectsItemShape, Qt::AscendingOrder);
            QList<QGraphicsItem*> visibleRight = scene->items(mice[i]->mapRectToScene(mice[i]->fieldOfVisionRight()),
                                                              Qt::IntersectsItemShape, Qt::AscendingOrder);

            foreach (QGraphicsItem* item, visibleFront) {
                if(Cheese* x = dynamic_cast<Cheese*>(item)) {
//                  qDebug() << "CHEESE";
                    inputs.push_back(100);
                    mice[i]->advanceBonus++;
                }
                else if(MouseTrap* x = dynamic_cast<MouseTrap*>(item)) {
//                  qDebug() << "TRAP";
                    inputs.push_back(-100);
                    mice[i]->advanceBonus--;
                }
                else if(WaterPool* x = dynamic_cast<WaterPool*>(item)) {
//                  qDebug() << "POOL";
                    inputs.push_back(-10);
                    mice[i]->advanceBonus--;
                }
                else {
                    continue;
                }
                inputs.push_back(item->pos().x());
                inputs.push_back(item->pos().y());
                break;
            }
            foreach (QGraphicsItem* item, visibleLeft) {
                if(Cheese* x = dynamic_cast<Cheese*>(item)) {
//                  qDebug() << "CHEESE";
                    inputs.push_back(100);
                }
                else if(MouseTrap* x = dynamic_cast<MouseTrap*>(item)) {
//                  qDebug() << "TRAP";
                    inputs.push_back(-100);
                    mice[i]->advanceBonus++;
                }
                else if(WaterPool* x = dynamic_cast<WaterPool*>(item)) {
//                  qDebug() << "POOL";
                    inputs.push_back(-10);
                    mice[i]->advanceBonus++;
                }
                else {
                    continue;
                }
                inputs.push_back(item->pos().x());
                inputs.push_back(item->pos().y());
                break;
            }

            foreach (QGraphicsItem* item, visibleRight) {
                if(Cheese* x = dynamic_cast<Cheese*>(item)) {
                    inputs.push_back(100);
                }
                else if(MouseTrap* x = dynamic_cast<MouseTrap*>(item)) {
                    inputs.push_back(-100);
                    mice[i]->advanceBonus++;
                }
                else if(WaterPool* x = dynamic_cast<WaterPool*>(item)) {
                    inputs.push_back(-10);
                    mice[i]->advanceBonus++;
                }
                else {
                    continue;
                }
                inputs.push_back(item->pos().x());
                inputs.push_back(item->pos().y());
                break;
            }


            while(inputs.size() < 12){
                inputs.push_back(0);
            }

//OUTPUTS


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
    // check for dead mice, determine best mouse
    for(size_t i = 0; i < mice.size(); i++){

        if(!mice[i]) { continue; }

        if(mice[i]->pos().y() >= cat->pos().y()){
            mice[i]->alive = false;
        }

        if (!mice[i]->alive){
            // A player died

            // fitness function
            double fitness = mice[i]->calcFitness();
            emit died(bId + i, fitness);
            delete mice[i];
            mice[i] = nullptr;
            numOfAlive--;
        }else{

            if(!mice[bestI] || mice[i]->pos().y() < mice[bestI]->pos().y()){
                bestI = i;
                drawGenome(genomes[bestI]);

            }

            double p = mice[bestI]->pos().y();
            if (leftBound->pos().y() > p + 600) {
                leftBound->setPos(leftBound->pos().x(), p);
                rightBound->setPos(rightBound->pos().x(), p);
            }
        }
    }

    if(numOfAlive == 0){
        delete nnView;
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
        if(WaterBound *b = dynamic_cast<WaterBound*>(item)) continue;
        item->setPos(item->pos().x(), item->pos().y() + cat->speed);
    }


    // run game normaly
    spawnObjects();
    focusBest();
    deleteObjects();
    makeDecisions();
}

void Game::focusBest(){

    // sceneWidth = 600, sceneHeight = 800
    setSceneRect(-300, mice[bestI]->y() - 400, 600, 800);
}

void Game::drawGenome(Genome *gen)
{

    nnView->scene()->clear();

    double d = 10;

    double offset = 3*d;
    double offsetL = 10*d;

    std::map<int, std::pair<int, int>> nodes;
    std::map<int, int> layers;

    for (size_t i = 0; i < gen->nodes.size(); i++){
        QGraphicsEllipseItem* node = new QGraphicsEllipseItem(0, 0, d, d);
        int l = gen->nodes[i]->layer;
        int id = gen->nodes[i]->id;

        if(layers.count(l) == 0){
            layers[l] = 1;
        }else{
            layers[l]++;
        }
        nodes[id] = std::make_pair(l * offsetL, layers[l] * offset);

        node->setPos(nodes[id].first, nodes[id].second);
        nnView->scene()->addItem(node);
    }
    layers.clear();

    for(size_t i = 0; i < gen->connections.size(); i++){


        int n1Id = gen->connections[i]->inNode->id;
        int n2Id = gen->connections[i]->outNode->id;

        double r = 0;
        double g = 0;
        double b = 0;


        if(gen->connections[i]->enabled){

            double w = gen->connections[i]->weight;
            if(w >= 0){
                g = w * 255;
            }else{
                r = -w * 255;
            }
        }else{
            b = 255;
        }


        QColor color(r,g,b);
        nnView->scene()->addLine(nodes[n1Id].first + d/2, nodes[n1Id].second + d/2,
                                 nodes[n2Id].first + d/2, nodes[n2Id].second + d/2, QPen(color));


    }

    nnView->scene()->setSceneRect(nnView->scene()->itemsBoundingRect());
    nnView->fitInView(nnView->scene()->sceneRect(), Qt::KeepAspectRatio);
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
