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

    int newNodeId;
    int newConnectionId;

    Genome* crossover(const Genome &other);

    void connectNodes();

    // This is used in Species class, when calculating excess and disjoint genes
    std::vector<ConnectionGene> connections;

signals:
    void connectionIdNeeded(Genome*, int fromNodeId, int toNodeId);

    void nodeIdNeeded(Genome*, int connectionId);


private:
    std::vector<NodeGene> nodes;
    // std::vector<ConnectionGene> connections;
    int numInputs;
    int numOutputs;
    int layers;
    int biasNodeId;
};

#endif // GENOME_H
