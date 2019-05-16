#include "species.h"
#include <random>
#include <QDebug>

Species::Species(Genome *p)
    : bestFitness{0}, averageFitness{0}, stagnantCoeff{0}, allowedReproduction{true},
      excessCoeff{1}, weightDiffCoeff{0.4}, compatibilityThreshold{3}
{
    genomes.push_back(p);
    bestFitness = p->fitness;       // Best fitness, because it is the only Genome
    representGenome = p;
}

bool Species::isSameSpecies(const Genome &genome)
{
    // We need to calculate the number of excess and disjoint genes between two input genomes
    // Plainly, we calculate the number of genes which don't match
    unsigned matchingExcessDisjoint = 0;

    // These are used a lot, move to variable
    size_t genomeConnSize = genome.connections.size();  // NOTE: In genome.h, connections were made public
    size_t representGenomeConnSize = representGenome->connections.size();

    for (size_t i = 0; i < genomeConnSize; i++) {
        for (size_t j = 0; j < representGenomeConnSize; j++) {
            if (genome.connections[i]->innovationNumber == representGenome->connections[j]->innovationNumber) {
                matchingExcessDisjoint++;
                break;
            }
        }
    }
    double excessAndDisjoint = genomeConnSize + representGenomeConnSize - 2*matchingExcessDisjoint;


    // Now we need to get the average weight difference between matching genes in the input genomes

    double averageWeightDiff = 0;
    if (genomeConnSize == 0 || representGenomeConnSize == 0) // Maybe these params should be calculated again
        averageWeightDiff = 0;

    unsigned matchingWeightDiff = 0;
    double totalDiff = 0;

    for (size_t i = 0; i < genomeConnSize; i++) {
        for (size_t j = 0; j < representGenomeConnSize; j++) {
            if (genome.connections[i]->innovationNumber == representGenome->connections[j]->innovationNumber) {
                matchingWeightDiff++;
                totalDiff += abs(genome.connections[i]->weight - representGenome->connections[j]->weight);
                break;
            }
        }
    }

    if (matchingWeightDiff == 0) // division by 0
        averageWeightDiff = 1; // something else ?
    else
        averageWeightDiff = totalDiff / matchingWeightDiff;

    // Calculate N - number of genes in the larger genome
    double largeGenomeNormalizer = genome.connections.size();
    if (largeGenomeNormalizer < 20) {    // Small genomes, both have less than 20 genes
      largeGenomeNormalizer = 1;
    }

    // Now calculate the compatibility function
    double tmp1 = excessCoeff * excessAndDisjoint / largeGenomeNormalizer;
    double compatibilityDistance = tmp1 + (weightDiffCoeff * averageWeightDiff);

//    qDebug() << "compatibilityDistance: " << compatibilityDistance;

    return (compatibilityThreshold > compatibilityDistance);
}

void Species::sortGenomesByFitness()
{
    std::sort(genomes.begin(), genomes.end(), [] (const auto &lhs, const auto &rhs) {
        return lhs->fitness > rhs->fitness;
    });

    // Checking if we should penalize the species if the fittest is not so fit
    if (genomes[0]->fitness > bestFitness) {        // OK - generation improved
        stagnantCoeff = 0;
        bestFitness = genomes[0]->fitness;
        representGenome = genomes[0];

    } else {
        // stagnantCoeff++;
        if (++stagnantCoeff > 15) {         // Too stale, don't reproduce
            allowedReproduction = false;
        }
    }
}

void Species::addToSpecies(Genome *genome)
{
    genomes.push_back(genome);
}

Genome* Species::createOffspring()
{
//    qDebug() << "species: createOffspring" << genomes.size();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0, 1);
    double r = dist(gen);
    Genome* offspring = nullptr;

    if (r < 0.25) {
//        qDebug() << "clone";
        offspring = selectParent()->clone();

    } else {
//        qDebug() << "parents";
        Genome *p1 = selectParent();
        Genome *p2 = selectParent();

        if (p1->fitness > p2->fitness) {
            offspring = p1->crossover(p2);
        } else {
            offspring = p2->crossover(p1);
        }
    }
    offspring->mutate();

//    qDebug() << "species: createOffspring end" << genomes.size();

    return offspring;
}

Genome* Species::selectParent()
{
    // Finding a Genome using roulette selection

//    qDebug() << "selectParent: start";
//    qDebug() << "genomes: " << genomes.size();
    double totalSpeciesFitness = std::accumulate(genomes.cbegin(), genomes.cend(), 0,
        [](double acc, Genome *p) {    // is an error
//            qDebug() << "fitness: " << p->fitness;
//            qDebug() << "acc: " << acc;
            return p->fitness + acc;
    });
//    qDebug() << "selectParent: accumulate" << genomes.size();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0, totalSpeciesFitness);
    double selectedValue = dist(gen);

    double fitnessSum = 0;
    for (unsigned i=0; i < genomes.size(); i++) {
        fitnessSum += genomes[i]->fitness;
        if (fitnessSum >= selectedValue)
            return genomes[i];
    }

    // This will never execute, but return value must exist. Pick a random Genome
    std::uniform_real_distribution<> dist1(0, genomes.size()-1);
//    qDebug() << "selectParent: fitness";
    return genomes[static_cast<size_t>(dist1(gen))];
}

void Species::explicitFitnessSharing()
{
    for (unsigned i=0; i < genomes.size(); i++) {
        genomes[i]->fitness /= genomes.size();
    }
}

void Species::decimateSpecies()
{
//    qDebug() << "decimate: " << genomes.size();
    if (genomes.size() <= 2)
        return;

    // Remove the second (worse) half of the species
    genomes.resize(genomes.size() / 2);

//    qDebug() << "decimate end: " << genomes.size();
}

// Calculate average fitness in the species (setter, not getter)
void Species::calcAverageFitness()
{
    double sum = std::accumulate(genomes.cbegin(), genomes.cend(), 0,
        [](double acc, Genome *p){    // Could be an error
            return p->fitness + acc;
    });

    averageFitness = sum/genomes.size();
}
