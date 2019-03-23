#ifndef CONNECTIONGENE_H
#define CONNECTIONGENE_H

class NodeGene;

class ConnectionGene
{
public:
    ConnectionGene(NodeGene *in, NodeGene *out, double weight, int innovationNumber);

    void mutateWeight();

// better solution later...
//private:
    NodeGene *inNode;
    NodeGene *outNode;
    double weight;
    bool enabled;
    int innovationNumber;
};

#endif // CONNECTIONGENE_H
