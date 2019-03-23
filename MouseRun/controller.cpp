#include "controller.h"
#include "game.h"
#include <QTime>

const int populationSize = 50;
// number of inputs and outputs for the genome (nn)
const int numInputs = 4;
const int numOutputs = 2;

Controller::Controller()
    : generationNum{0},
      nextConnId{0},
      nextNodeId{numInputs + numOutputs + 1}

{
    // create initial population
    for(int i = 0; i < populationSize; i++) {
        Genome *genome = new Genome(numInputs, numOutputs);
        population.push_back(genome);

        connect(genome, SIGNAL(nodeIdNeeded(Genome*, int)),
                this, SLOT(getNodeId(Genome*, int)),
                Qt::DirectConnection);

        connect(genome, SIGNAL(connIdNeeded(Genome*, int)),
                this, SLOT(getConnId(Genome*, int)),
                Qt::DirectConnection);
    }

    evolve();
}


void Controller::getNodeId(Genome *genome, int connectionId)
{
    auto search = mapNode.find(connectionId);
    if(search == mapNode.end()) {
        mapNode[connectionId] = nextNodeId++;
    }
    genome->newNodeId = mapNode[connectionId];
}

void Controller::getConnId(Genome *genome, int fromNodeId, int toNodeId)
{
    auto key = std::make_pair(fromNodeId, toNodeId);
    auto search = mapConn.find(key);
    if(search == mapConn.end()) {
        mapConn[key] = nextConnId++;
    }
    genome->newConnectionId = mapConn[key];
}

void Controller::evolve()
{
    // Calculate fitness as the time a player stays alive
    for(auto genome : population) {

        QTime time;
        time.start();
        Game game(genome);
        genome->fitness = time.elapsed();

    }

    // Create new population
    std::vector<Genome*> newPopulation;

    population = newPopulation;
    evolve();
    // TODO stop criteria

}
