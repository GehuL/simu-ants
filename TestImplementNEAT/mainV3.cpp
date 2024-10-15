#include <iostream>
#include <vector>
#include "Genome.h"
#include "NeuralNetwork.h"
#include "Utils.h"
#include "neat.h"
#include "ComputeFitness.h"
#include "Population.h"

int main() {
    // Configuration NEAT
    NeatConfig config;

    // Générateur aléatoire pour évaluer la fitness
    RNG rng;
    ComputeFitness compute_fitness(rng);

    // Créez une population initiale
    Population population(config, rng);

    // Nombre maximal de générations
    int max_generations = 5;

    // Exécutez l'algorithme NEAT
    auto best_individual = population.runV2(compute_fitness, max_generations);

    // Affichez le meilleur individu trouvé
    std::cout << "Best individual after " << max_generations << " generations has fitness: " << best_individual.fitness << std::endl;

    return 0;
}
