#include "connectiongene.h"
#include "nodegene.h"

#include <random>
#include <algorithm>

ConnectionGene::ConnectionGene(NodeGene *in, NodeGene *out, double weight, int innovationNumber)
    : inNode{in}, outNode{out}, weight{weight}, enabled{true}, innovationNumber{innovationNumber}
{
}

void ConnectionGene::mutateWeight()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist1(0, 1);
    double r = dist1(gen);
    if(r < 0.1) {
        // total change of the weight
        std::uniform_real_distribution<> dist2(-1, 1);
        weight = dist2(gen);
    } else {
        // slight modification of the weight
        std::uniform_real_distribution<> dist3(-1.0 / 50, 1.0 / 50);
        weight += dist3(gen);
        weight = std::min(weight, 1.0);
        weight = std::max(weight, -1.0);
    }
}
