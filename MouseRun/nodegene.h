#ifndef NODEGENE_H
#define NODEGENE_H

#include <vector>

class ConnectionGene;

class NodeGene
{
public:
    NodeGene(int id, int layer);

    void activate();

    bool isConnectedTo(NodeGene *other);

// better solution later...
//private:
    int id;
    int layer;
    double inputSum;
    double outputValue;
    std::vector<ConnectionGene*> outputConnections;

    double activationFunction(double x);
};

#endif // NODEGENE_H
