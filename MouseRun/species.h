#ifndef SPECIES_H
#define SPECIES_H

#include "player.h"

class Species
{
public:
    Species(Genome *g);

    bool isSameSpecies(const Genome &gen);  // Is current genome in the same species as 'gen'

    void calcAverageFitness();              // Average fitness (setter, not getter)

    void sortSpeciesByFitness();

    Genome* selectParent();              // Select a genome from this species

    // TODO
    Genome* createOffspring();

    void decimateSpecies();                 // Get rid of the unfit genomes in this species

    void explicitFitnessSharing();          // Stops species from becoming too big

private:
    std::vector<Genome*> genomes;
    Genome *bestGenome;     // necessary ?

    Genome *representGenome;    // pointer ?

    double bestFitness;
    double averageFitness;
    unsigned stagnantCoeff;             // How many generations without an improvement
    bool allowedReproduction;           // If the staleness is high, species should not reproduce

    // Parameters for compatibility function
    double excessCoeff;
    double weightDiffCoeff;
    double compatibilityThreshold;
};

#endif // SPECIES_H
