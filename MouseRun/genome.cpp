#include "genome.h"

#include <algorithm>
#include <random>
#include <QDebug>

Genome::Genome(int inputs, int outputs)
    : numInputs{inputs}, numOutputs{outputs}
{
    // input and output layer at the beginning
    layers = 2;

    int nextNodeId = 0;
    // input nodes - [0, inputs)
    for(; nextNodeId < numInputs; nextNodeId++) {
        nodes.push_back(new NodeGene(nextNodeId, 0));
    }

    // output nodes - [inputs, inputs + outputs)
    for(; nextNodeId < numInputs + numOutputs; nextNodeId++) {
        nodes.push_back(new NodeGene(nextNodeId, 1));
    }

    // bias node
    biasNodeId = nextNodeId;
    nodes.push_back(new NodeGene(biasNodeId, 0));
}

std::vector<double> Genome::feedForward(std::vector<double> inputValues)
{
    // set input values for input nodes
    for(int i = 0; i < numInputs; i++) {
        nodes[i]->outputValue = inputValues[i];
    }
    nodes[biasNodeId]->outputValue = 1;

//    qDebug() << nodes;
    // nodes should be activated layer by layer
    std::vector<NodeGene*> sortedNodes = nodes;
    std::sort(sortedNodes.begin(), sortedNodes.end(),
              [](NodeGene *a, NodeGene *b){return a->layer < b->layer;});



    for(auto&& node : sortedNodes) {
//        qDebug() << "feedForward: " << node->outputConnections.size();
        node->activate();
    }

    // get output values of output nodes
    std::vector<double> result(numOutputs);
    for(int i = numInputs; i < numInputs + numOutputs; i++) {
        result[i - numInputs] = nodes[i]->outputValue;
    }

    // reset inputSum
    for(auto&& node : nodes) {
        node->inputSum = 0;
//        qDebug() << "feedForward 2: " << node->outputConnections.size();
    }

    return result;
}

void Genome::mutate()
{
//    qDebug() << "mutate genome";
//    qDebug() << "mutate: " << connections.size();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0, 1);
    double r = dist(gen);

    if(r < 0.8) {
        // 80% - weights mutation
//        qDebug() << "weights mutation";
        if(connections.empty()) {
            addConnection();
        }
        for(auto&& conn : connections) {
            conn->mutateWeight();
        }
    } else if(r < 0.9) {
        // 10% - new connection
//        qDebug() << "addConnection mutation";
        addConnection();
        // 10% X (50%)^N to add N connections
        while(dist(gen) < 0.5){
            addConnection();
        }
    } else if(r < 0.95) {
//        qDebug() << "addNode mutation";
        // 5% - new node
        addNode();
        // 5% X (25%)^N to add N nodes
        while(dist(gen) < 0.25){
            addNode();
        }
    }
}

void Genome::addNode()
{
    // pick a random connection
    std::random_device rd;
    std::mt19937 gen(rd());
    // switch to addConnection if there are no connections
    if(connections.size() == 0) {
        addConnection();
        return;
    }
    std::uniform_int_distribution<> dist(0, connections.size() - 1);
    ConnectionGene *connection = connections[dist(gen)];
    // not disconnecting bias
    if(connection->inNode->id == biasNodeId && connections.size() == 1) {
        addConnection();
        return;
    }
    // pick a nonbias inNode
    while(connection->inNode->id == biasNodeId) {
        connection = connections[dist(gen)];
    }
    // disable picked connection
    connection->enabled = false;

//    qDebug() << "addNode: signal";
    emit nodeIdNeeded(this, connection->innovationNumber);
//    qDebug() << "posle signala: " << newNodeId;
    // they are connected with Qt::DirectConnection, so the slot will execute by now - newNodeId will be correct
    NodeGene *newNode = new NodeGene(newNodeId, connection->inNode->layer + 1);

    emit connectionIdNeeded(this, connection->inNode->id, newNodeId);
//    qDebug() << "posle 2. signala: " << newConnectionId;
    // newConnectionId is set  - create connection between old inNode and new node
    ConnectionGene *connection1 = new ConnectionGene(connection->inNode, newNode, 1, newConnectionId);
    connections.push_back(connection1);

    emit connectionIdNeeded(this, newNodeId, connection->outNode->id);
//    qDebug() << "posle 3. signala: " << newConnectionId;
    // newConnectionId is set - create connection between new node and old outNode
    ConnectionGene *connection2 = new ConnectionGene(newNode, connection->outNode, connection->weight, newConnectionId);
    connections.push_back(connection2);

    emit connectionIdNeeded(this, biasNodeId, newNodeId);
//    qDebug() << "posle 4. signala: " << newConnectionId;
    // newConnectionId is set - create connection between bias node and new node
    ConnectionGene *connection3 = new ConnectionGene(nodes[biasNodeId], newNode, 0, newConnectionId);
    connections.push_back(connection3);

    if(newNode->layer == connection->outNode->layer) {
        for(size_t i = 0; i < nodes.size(); i++) {
            if(nodes[i]->layer >= newNode->layer) {
                nodes[i]->layer++;
            }
        }
        layers++;
    }
    nodes.push_back(newNode);

//    qDebug() << "pred kraj";
    // update changed connections
//    connection->inNode->outputConnections.push_back(connection1);
//    newNode->outputConnections.push_back(connection2);
//    nodes[biasNodeId]->outputConnections.push_back(connection3);

    connectNodes();
}

void Genome::addConnection()
{
//    qDebug() << "fja: addConnection" << connections.size();
//    for(size_t i = 0; i < nodes.size(); i++) {
//        qDebug() << nodes[i]->outputConnections.size();
//    }
    // pick two nodes
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, nodes.size() - 1);
    NodeGene *fromNode = nodes[dist(gen)];
    NodeGene *toNode = nodes[dist(gen)];

    // nije nula?????
//    qDebug() << "fromNode: " << fromNode->outputConnections.size();
//    qDebug() << "toNode: " << toNode->outputConnections.size();


//    if(fromNode->layer == toNode->layer) {
//        qDebug() << "lejers";
//    }
//    if(fromNode->isConnectedTo(toNode)) {
//        qDebug() << "povezani";
//    }
//    if(toNode->isConnectedTo(fromNode)) {
//        qDebug() << "povezani 2";
//    }


    while(fromNode->layer == toNode->layer || fromNode->isConnectedTo(toNode) || toNode->isConnectedTo(fromNode)) {
//        qDebug() << "while...";
        fromNode = nodes[dist(gen)];
        toNode = nodes[dist(gen)];
    }

    // swap if needed
    if(fromNode->layer > toNode->layer) {
        std::swap(fromNode, toNode);
    }

//    qDebug() << "pre EMIT-a: fromNode->id = " << fromNode->id << ", toNode->id = " << toNode->id;
    emit connectionIdNeeded(this, fromNode->id, toNode->id);
//    qDebug() << "posle EMIT-a: newConnectionId = " << newConnectionId;
    std::uniform_real_distribution<> distReal(-1, 1);
    ConnectionGene *connection = new ConnectionGene(fromNode, toNode, distReal(gen), newConnectionId);
    connections.push_back(connection);

    // update changed connections
//    fromNode->outputConnections.push_back(connection);

    connectNodes();
}

Genome* Genome::crossover(Genome *other)
{
    Genome* child = new Genome(numInputs, numOutputs);
    child->layers = layers;
    child->biasNodeId = biasNodeId;
    // all nodes are inherited from more fit parent - this Genome
    child->nodes = nodes;

    for(auto&& c1 : connections) {
        int index = -1;
        for(int i = 0; i < other->connections.size(); i++) {
            if(c1->innovationNumber == other->connections[i]->innovationNumber) {
                index = i;
            }
        }

        if(index != -1) {
            // matching genes - inherit from random parent

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dist(0, 1);

            bool enable = true;

            // if connection is disabled in both parents, there is 75% chance that it is disabled in the child
            if(!c1->enabled || !other->connections[index]->enabled) {
                if(dist(gen) < 0.75) {
                    enable = false;
                }
            }

            if(dist(gen) < 0.5) {
                ConnectionGene *childConnection = new ConnectionGene(c1->inNode, c1->outNode, c1->weight, c1->innovationNumber);
                childConnection->enabled = enable;
                child->connections.push_back(childConnection);
            } else {
                ConnectionGene *childConnection = new ConnectionGene
                                                (other->connections[index]->inNode, other->connections[index]->outNode,
                                               other->connections[index]->weight, other->connections[index]->innovationNumber);
                childConnection->enabled = enable;
                child->connections.push_back(childConnection);
            }

        } else {
            // excess or disjoint gene - inherit from the more fit parent - this parent

            ConnectionGene *childConnection = new ConnectionGene(c1->inNode, c1->outNode, c1->weight, c1->innovationNumber);
            childConnection->enabled = c1->enabled;
            child->connections.push_back(childConnection);
        }
    }

    child->connectNodes();

    return child;
}

Genome *Genome::clone()
{
    Genome* genome = new Genome(numInputs, numOutputs);

    genome->nodes = nodes;
    genome->layers = layers;
    genome->fitness = fitness;
    genome->newNodeId = newNodeId;
    genome->biasNodeId = biasNodeId;
    genome->connections = connections;
    genome->newConnectionId = newConnectionId;

    genome->connectNodes();

    return genome;
}

void Genome::connectNodes()
{
    for(int i = 0; i < nodes.size(); i++) {
        nodes[i]->outputConnections.clear();
    }

    for(int i = 0; i < connections.size(); i++) {
        connections[i]->inNode->outputConnections.push_back(connections[i]);
    }
}
