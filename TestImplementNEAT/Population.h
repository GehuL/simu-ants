#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include "Genome.h"
#include "RNG.h"
#include "NeatConfig.h"
#include "neat.h"

class Population {
public:
    Population(NeatConfig config, RNG &rng);

    // Méthode run avec un template pour la fonction de fitness
    template <typename FitnessFn>
    neat::Individual run(FitnessFn compute_fitness, int num_generations);

private:
    NeatConfig config;
    RNG &rng;
    std::vector<neat::Individual> individuals;  // Vecteur d'individus
    neat::Individual best_individual;

    int next_genome_id();
    Genome new_genome();
    neat::NeuronGene new_neuron(int neuron_id);
    neat::LinkGene new_link(int input_id, int output_id);

    std::vector<neat::Individual> reproduce();
    void mutate(Genome &genome);
    void update_best();
    std::vector<neat::Individual> sort_individuals_by_fitness(const std::vector<neat::Individual>& individuals);
};

// Définition de la méthode run avec un template
template <typename FitnessFn>
neat::Individual Population::run(FitnessFn compute_fitness, int num_generations) {
    for (int generation = 0; generation < num_generations; ++generation) {
        // Calcul de la fitness pour chaque individu
        for (auto &individual : individuals) {
            if (!individual.fitness_computed) {
                individual.fitness = compute_fitness(individual.genome);
                individual.fitness_computed = true;
            }
        }

        // Met à jour le meilleur individu
        update_best();

        // Générer la nouvelle génération
        individuals = reproduce();
    }

    // Retourner le meilleur individu après toutes les générations
    return best_individual;
}

#endif // POPULATION_H
