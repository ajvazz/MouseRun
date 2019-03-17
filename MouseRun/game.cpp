#include "game.h"

#include "cheese.h"
#include "mousetrap.h"
#include "waterpool.h"
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>

const int populationSize = 50;
// number of inputs and outputs for the genome (nn)
const int numInputs = 4;
const int numOutputs = 2;

Game::Game()
    : nextConnId{0},
      nextNodeId{numInputs + numOutputs + 1}
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

    setBackgroundBrush(QBrush(QColor(55,205,55)));

    // Start the game
    start();
}

void Game::start()
{
    // Spawn player
    player = new Player(numInputs, numOutputs);
    scene->addItem(player);

    // Spawn cat
    cat = new Cat();
    cat->setPos(0, 1000);
    scene->addItem(cat);

    // create population of players
//    for(int i = 0; i < populationSize; i++) {
//        Player *player = new Player(numInputs, numOutputs);
//        population.push_back(player);
//        scene->addItem(player);
//    }
//    for(auto&& p : population) {
//        connect(&p->genome, SIGNAL(nodeIdNeeded(Genome*, int)),
//                this, SLOT(getNodeId(Genome*, int)),
//                Qt::DirectConnection);
//    }


    // Update the Game every 15 ms
    static QTimer updateTimer;
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    updateTimer.start(15);

    // Spawn new objects every 500 ms
//    static QTimer spawnTimer;
//    connect(&spawnTimer, SIGNAL(timeout()), this, SLOT(spawnObjects()));
//    spawnTimer.start(1500);

    // Setup left and right bound
    boundW = 500;
    leftBound  = new WaterBound(1500, boundW);
    rightBound = new WaterBound(1500, boundW);

    leftBound->setPos(-boundW, 0);
    rightBound->setPos(boundW, 0);

    scene->addItem(leftBound);
    scene->addItem(rightBound);

    // Show the scene
    show();
}

// Disable player deselection
void Game::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}
// Disable player deselection
void Game::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void Game::update()
{
    if (!player){
        // The player died
        // restart()
        exit(0);
    }

    spawnObjects();
    deleteObjects();

    // Move the cat
    QPointF v = player->pos() - cat->pos();
    qreal d = sqrt(v.x() * v.x() + v.y() * v.y());
    QPointF dir = v / d;
    cat->setPos(cat->pos() + dir * cat->speed);

    // Moving background picture
    // Ovo je posao za Satanu ^^^^

//    if (leftBound->pos().y() > 200) {
////        setBackgroundBrush(QBrush(QColor(55,205,55)));
//        setBackgroundBrush(QBrush(QPixmap(":/img/grass.jpg")));
//        // drawBackground
//    }

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

    qDebug() << scene->items().size();
}


void Game::spawnObjects()
{
    if (scene->items().length() >= 25) return;

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
                 player->pos().y() - QRandomGenerator::global()->bounded(300, 1000));

    scene->addItem(item);
}

void Game::deleteObjects()
{
    foreach (QGraphicsItem* item, scene->items()) {
        if (item->pos().y() > 350){
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

void Game::getNodeId(Genome *genome, int connectionId)
{
    auto search = mapNode.find(connectionId);
    if(search == mapNode.end()) {
        mapNode[connectionId] = nextNodeId++;
    }
    genome->newNodeId = mapNode[connectionId];
}

void Game::getConnId(Genome *genome, int fromNodeId, int toNodeId)
{
    auto key = std::make_pair(fromNodeId, toNodeId);
    auto search = mapConn.find(key);
    if(search == mapConn.end()) {
        mapConn[key] = nextConnId++;
    }
    genome->newConnectionId = mapConn[key];
}
