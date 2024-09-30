#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include "Genome.h"
#include "RNG.h"
#include "NeatConfig.h"
#include "neat.h"

// Constantes à définir selon votre logique
const bool kFitnessComputed = true;

class Population {
public:
    Population(NeatConfig config, RNG &rng);

    // Ajoutez des méthodes publiques supplémentaires si nécessaire
    // par exemple : auto run(ComputeFitness &compute_fitness, int num_generations);

private:
    NeatConfig config;
    RNG &rng;
    std::vector<neat::Individual> individuals;  // Vecteur d'individus

    int next_genome_id();
    Genome new_genome();
    neat::NeuronGene new_neuron(int neuron_id);
    neat::LinkGene new_link(int input_id, int output_id);

    neat::Individual run(ComputeFitness &compute_fitness, int num_generations);
};

#endif // POPULATION_H
