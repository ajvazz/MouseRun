#include "nodegene.h"
#include "connectiongene.h"

#include <cmath>
#include <QDebug>

NodeGene::NodeGene(int id, int layer)
    : id{id}, layer{layer}, inputSum{0}, outputValue{0}
{

}

// node sends its output value to all connected nodes
void NodeGene::activate()
{
    // input layer has no inputSum
    if(layer != 0) {
        outputValue = activationFunction(inputSum);
    }

    // nula je
//    qDebug() << "node activate: " << outputConnections.size();
    for(int i = 0; i < outputConnections.size(); i++) {
        if(outputConnections[i]->enabled){
            outputConnections[i]->outNode->inputSum += outputConnections[i]->weight * outputValue;
        }
    }
}

// returns true if this node is connected to other node
bool NodeGene::isConnectedTo(NodeGene *other)
{
    for(int i = 0; i < outputConnections.size(); i++) {
//        qDebug() << outputConnections[i]->outNode->id;
        if(outputConnections[i]->outNode->id == other->id) {
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
