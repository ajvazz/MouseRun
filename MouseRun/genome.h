#ifndef GENOME_H
#define GENOME_H

#include <vector>
#include <QObject>

#include "nodegene.h"
#include "connectiongene.h"

class Genome : public QObject
{
    Q_OBJECT

public:
    Genome(int input, int output);

    std::vector<double> feedForward(std::vector<double> inputValues);

    void mutate();

    void addNode();

    void addConnection();

    Genome* crossover(Genome *other);

    Genome* clone();

    void connectNodes();

    int newNodeId;
    int newConnectionId;

    std::vector<NodeGene*> nodes;
    std::vector<ConnectionGene*> connections;
    double fitness;
    int numInputs;
    int numOutputs;
    int layers;
    int biasNodeId;

signals:
    void connectionIdNeeded(Genome*, int fromNodeId, int toNodeId);

    void nodeIdNeeded(Genome*, int connectionId);

};

#endif // GENOME_H
