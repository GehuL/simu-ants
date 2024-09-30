#ifndef COMPUTEFITNESS_H
#define COMPUTEFITNESS_H

#include "RNG.h"  // Inclure la classe RNG (générateur de nombres aléatoires)
#include "Genome.h"  // Inclure la définition du Genome

class ComputeFitness {
public:
    // Constructeur qui prend un générateur RNG en référence
    ComputeFitness(RNG &rng);

    // Méthode pour évaluer la fitness d'un génome
    double evaluate(const Genome &genome);

private:
    RNG &rng;  // Référence au générateur RNG utilisé pour l'évaluation
};

#endif // COMPUTEFITNESS_H
