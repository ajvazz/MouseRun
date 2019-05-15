#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "species.h"
#include "genome.h"

#include <QObject>
#include <QTime>

class Controller : public QObject
{
    Q_OBJECT

public:
    Controller();

public slots:
    void getNodeId(Genome* genome, int connectionId);

    void getConnId(Genome* genome, int fromNodeId, int toNodeId);

    void calculateFitness(int i);

private:
    // population of genetic algorithm
    std::vector<Genome*> population;

    std::vector<Species*> species;

    // current generation
    int generationNum;

    // Every structural mutation must be saved - innovation numbers (ids)
    // of both nodes and connections must be unique for entire generation

    // maps ids of two nodes' ids to id of connection between them - used for addConnection mutation
    std::map<std::pair<int, int>, int> mapConn;

    // if given key doesn't exist in the mapConn, nextConnId is incremented and set as value in the map
    int nextConnId;

    // maps connection id between two nodes to id of new node between them - used for addNode mutation
    std::map<int, int> mapNode;

    // if given key doesn't exist in the mapNode, nextNodeId is incremented and set as value in the map
    int nextNodeId;

    QTime time;

    int numGenomesDone;

    int numOfGenerations;

    void evolve();

    void runGeneration();
};

#endif // CONTROLLER_H
