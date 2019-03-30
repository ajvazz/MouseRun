#include "species.h"
#include <random>

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
    double compatibilityDistance;
    double excessAndDisjoint = 0;
    double averageWeightDiff = 0;

    // We need to calculate the number of excess and disjoint genes between two input genomes
    // Plainly, we calculate the number of genes which don't match
    unsigned matchingExcessDisjoint = 0;

    // These are used a lot, move to variable
    size_t genomeConnSize = genome.connections.size();  // NOTE: In genome.h, connections were made public
    size_t representGenomeConnSize = representGenome->connections.size();

    for (size_t i = 0; i < genomeConnSize; i++) {
        for (size_t j = 0; j < representGenomeConnSize; j++) {
            if (genome.connections[i].innovationNumber == representGenome->connections[j].innovationNumber) {
                matchingExcessDisjoint++;
                break;
            }
        }
    }
    excessAndDisjoint = genomeConnSize + representGenomeConnSize - 2*matchingExcessDisjoint;


    // Now we need to get the average weight difference between matching genes in the input genomes

    if (genomeConnSize == 0 || representGenomeConnSize == 0) // Maybe these params should be calculated again
        averageWeightDiff = 0;

    unsigned matchingWeightDiff = 0;
    double totalDiff = 0;

    for (size_t i = 0; i < genomeConnSize; i++) {
        for (size_t j = 0; j < representGenomeConnSize; j++) {
            if (genome.connections[i].innovationNumber == representGenome->connections[j].innovationNumber) {
                matchingWeightDiff++;
                totalDiff += abs(genome.connections[i].weight - representGenome->connections[j].weight);
                break;
            }
        }
    }

    if (matchingWeightDiff == 0) // division by 0
        averageWeightDiff = 100; // something else ?
    else
        averageWeightDiff = totalDiff / matchingWeightDiff;

    // Calculate N - number of genes in the larger genome
    double largeGenomeNormalizer = genome.connections.size();
    if (largeGenomeNormalizer < 20) {    // Small genomes, both have less than 20 genes
      largeGenomeNormalizer = 1;
    }

    // Now calculate the compatibility function
    double tmp1 = excessCoeff * excessAndDisjoint / largeGenomeNormalizer;
    compatibilityDistance = tmp1 + (weightDiffCoeff* averageWeightDiff);

    return (compatibilityThreshold > compatibilityDistance);
}

void Species::sortGenomesByFitness()
{
    // NOTE: may have an error, had nothing to test this on
    std::sort(genomes.begin(), genomes.end(), [] (const auto &lhs, const auto &rhs) {
        return lhs->fitness > rhs->fitness;     // Or '<' idk
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


// Due to many errors, left unimplemented
Genome* Species::createOffspring()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0, 1);
    double r = dist(gen);
    Genome* offspring = nullptr;

    if (r < 0.25) {
        offspring = selectParent()->clone();
    } else {

        Genome *p1 = selectParent();
        Genome *p2 = selectParent();

        if (p1->fitness > p2->fitness) {
            offspring = p1->crossover(p2);
        } else {
            offspring = p2->crossover(p1);
        }
    }
    offspring->mutate();

    return offspring;
}

Genome* Species::selectParent()
{
    // Finding a Genome using roulette selection

    double totalSpeciesFitness = std::accumulate(genomes.cbegin(), genomes.cend(), 0,
        [](double acc, Genome *p){    // Could be an error
            return p->fitness + acc;
    });

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
    if (genomes.size() <= 2)
        return;

    // Remove the second (worse) half of the species
    genomes.erase(genomes.cbegin() + genomes.size()/2, genomes.cend());
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
