#include "nodegene.h"
#include "connectiongene.h"

#include <cmath>

NodeGene::NodeGene(int id, int layer)
    : id{id}, layer{layer}, inputSum{0}
{

}

// node sends its output value to all connected nodes
void NodeGene::activate()
{
    // input layer has no inputSum
    if(layer != 0) {
        outputValue = activationFunction(inputSum);
    }

    for(const auto& c : outputConnections) {
        c.outNode->inputSum += c.weight * outputValue;
    }
}

// returns true if this node is connected to other node
bool NodeGene::isConnectedTo(const NodeGene &other)
{
    for(const auto& c : outputConnections) {
        if(c.outNode->id == other.id) {
            return true;
        }
    }
    return false;
}

// modified sigmoidal function
double NodeGene::activationFunction(double x)
{
    return 1 / (1 + std::exp(-4.9 * x));
}
