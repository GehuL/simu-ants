#include <iostream>
#include <vector>
#include "Genome.h"           // Inclure la définition de Genome
#include "NeuralNetwork.h"    // Inclure la définition de FeedForwardNeuralNetwork
#include "Utils.h"            // Inclure la définition de save
#include "neat.h"             // Inclure neat.h pour les définitions complètes
#include "ComputeFitness.h"   // Inclure ComputeFitness
#include "Population.h"       // Inclure Population

int main() {
    // Configuration NEAT
    NeatConfig config;

    // Générateur aléatoire pour évaluer la fitness
    RNG rng;
    ComputeFitness compute_fitness(rng);

    // Créez une population initiale
    Population population(config, rng);

    // Nombre maximal de générations
    int max_generations = 100;

    // Exécutez plusieurs générations d'évaluation
    for (int generation = 0; generation < max_generations; generation++) {
        for (neat::Individual& individual : population.get_individuals()) {
            FeedForwardNeuralNetwork nn = create_from_genome(individual.genome);
            std::vector<double> game_state = get_game_state(); // Récupérez l'état du jeu
            std::vector<double> actions = nn.activate(game_state); // Prédisez les actions

            // Exécutez les actions dans le jeu
            perform_actions(actions);

            // Évaluez la performance de l'individu
            individual.fitness = compute_fitness.evaluate(individual.genome);
            individual.fitness_computed = true;
        }

        //population.evolve(); // Appliquez NEAT pour créer une nouvelle génération
    }

    return 0;
}
