#ifndef SPECIES_H
#define SPECIES_H

#include "player.h"

class Species
{
public:
    Species(Genome *g);

    bool isSameSpecies(const Genome &gen);  // Is current genome in the same species as 'gen'

    void calcAverageFitness();              // Average fitness (setter, not getter)

    void sortGenomesByFitness();

    void addToSpecies(Genome* genome);

    Genome* selectParent();              // Select a genome from this species

    Genome* createOffspring();

    void decimateSpecies();                 // Get rid of the unfit genomes in this species

    void explicitFitnessSharing();          // Stops species from becoming too big

    bool allowedReproduction;           // If the staleness is high, species should not reproduce


    Genome *representGenome;    // pointer ?

    double bestFitness;
    double averageFitness;
    unsigned stagnantCoeff;
    std::vector<Genome*> genomes;
private:
           // How many generations without an improvement

    // Parameters for compatibility function
    double excessCoeff;
    double weightDiffCoeff;
    double compatibilityThreshold;
};

#endif // SPECIES_H
