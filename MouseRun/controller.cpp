#include "controller.h"
#include "game.h"
#include <cmath>
#include <QTime>
#include <QDebug>

const int populationSize = 1000;
const int batchSize = 100;

// number of inputs and outputs for the genome (nn)
const int numInputs = 10;
const int numOutputs = 4;

Controller::Controller()
    : generationNum{0},
      nextConnId{0},
      nextNodeId{numInputs + numOutputs + 1},
      numGenomesDone{0},
      numOfGenerations{999999999}
{
    // create initial population
    for(int i = 0; i < populationSize; i++) {

        Genome *genome = new Genome(numInputs, numOutputs);
        population.push_back(genome);

        connect(genome, SIGNAL(nodeIdNeeded(Genome*, int)),
                this, SLOT(getNodeId(Genome*, int)),
                Qt::DirectConnection);

        connect(genome, SIGNAL(connectionIdNeeded(Genome*, int, int)),
                this, SLOT(getConnId(Genome*, int, int)),
                Qt::DirectConnection);
    }


//    for(int i = 0; i < numOfGenerations; i++) {
//        runGeneration();
//        time.restart();
//        numGenomesDone = 0;
//    }

//    qDebug() << "runGeneration";
    runGeneration(0);
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

void Controller::calculateFitness(size_t i, double score)
{
    population[i]->fitness = score;
    qDebug() << "Brain: " << i
             << "Fitness: " << population[i]->fitness
             << "Synapses: " << population[i]->connections.size()
             << "Neurons: " << population[i]->nodes.size()
             << "Layers: " << population[i]->layers;
//    for(int j = 0; j < population[i]->connections.size(); j++) {
//        qDebug() << "innonum: " << population[i]->connections[j]->innovationNumber << population[i]->connections[j]->weight;
//    }
    numGenomesDone++;
    if(numGenomesDone == populationSize) {
        generationNum++;
        evolve();
    }else if(numGenomesDone % batchSize == 0){
        runGeneration(numGenomesDone / batchSize);
    }
}

void Controller::runGeneration(int bNum)
{
    qDebug() << "Generation:" << generationNum << "Batch:" << bNum;
    // Calculate fitness as the time a player stays alive
    std::vector<Genome*> batch;
    for(size_t i = 0; i < batchSize; i++) {
        batch.push_back(population[bNum * batchSize + i]);
    }
    Game *game = new Game(batch, bNum * batchSize);
    connect(game, SIGNAL(died(size_t, double)),
            this, SLOT(calculateFitness(size_t, double)));
}

void Controller::evolve()
{
//    qDebug() << "evolve";


    // Speciate
    for(size_t i = 0; i < population.size(); i++) {
        bool newSpecies = true;
        for(size_t j = 0; j < species.size(); j++) {
            if(species[j]->isSameSpecies(*population[i])) {
                species[j]->addToSpecies(population[i]);
                newSpecies = false;
//                qDebug() << "same species";
                break;
            }
        }

        if (newSpecies) {
//            qDebug() << "new species";
            species.push_back(new Species(population[i]));
        }
    }

    // Create new population
    std::vector<Genome*> newPopulation;



    double averageFitnessSum = 0;

    for(size_t i = 0; i < species.size(); i++) {


        if (!species[i]->allowedReproduction) {
//            qDebug() << "jaoov";
            continue;
            // maybe delete this species
        }
        species[i]->sortGenomesByFitness();
        species[i]->decimateSpecies();
        species[i]->calcAverageFitness();
        species[i]->explicitFitnessSharing();
        averageFitnessSum += species[i]->averageFitness;
    }

    std::sort(species.begin(), species.end(),
              [](const Species *a, const Species *b){return a->averageFitness > b->averageFitness;} );

//    qDebug() << species.size();
    for(size_t i = 0; i < species.size(); i++) {


        if (!species[i]->allowedReproduction) {
//            qDebug() << "jalov";
            continue;
            // maybe delete this species
        }
//        qDebug() << "newpopsize: " << newPopulation.size();

        newPopulation.push_back(species[i]->representGenome->clone());

        int numOffspring = std::floor(species[i]->averageFitness / averageFitnessSum * populationSize) - 1;

//        qDebug() << "numOffspring: " << numOffspring;
//        qDebug() << "SACE UNUTRASNJI FOR";
        for(int j = 0; j < numOffspring; j++) {
//            qDebug() << "FOR: " << species[i]->genomes.size();
            newPopulation.push_back(species[i]->createOffspring());
//            qDebug() << "posle FOR-a: " << species[i]->genomes.size();
        }
//        qDebug() << "GOTOV UNUTRASNJI FOR";
    }

    // the rest chosen from best species
    while(newPopulation.size() < populationSize) {
//        qDebug() << "fali nesto";
        newPopulation.push_back(species[0]->createOffspring());
    }

    // TODO interspecies breeding (0.001 prob)

    population.clear();
    for(size_t i = 0; i < populationSize; i++) {
        population.push_back(newPopulation[i]);
    }

//    qDebug() << "novi: ";
    // connect newly created population of genomes to slots
    for(size_t i = 0; i < populationSize; i++) {
        connect(population[i], SIGNAL(nodeIdNeeded(Genome*, int)),
                this, SLOT(getNodeId(Genome*, int)),
                Qt::DirectConnection);

        connect(population[i], SIGNAL(connectionIdNeeded(Genome*, int, int)),
                this, SLOT(getConnId(Genome*, int, int)),
                Qt::DirectConnection);

//        qDebug() << population[i];
    }

    // mutate...
    for(size_t i = 0; i < populationSize; i++) {
//        qDebug() << "petlja: mutate: " << i;
        population[i]->mutate();
    }

    if(--numOfGenerations > 0) {
        numGenomesDone = 0;
        runGeneration(0);
    }
}
