#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include "Genome.h"
#include "RNG.h"
#include "NeatConfig.h"
#include "neat.h"
#include "NeuralNetwork.h"
#include"Utils.h"
#include <iostream>

class Population {
public:
    Population(NeatConfig config,  RNG &rng);

    // Méthode run avec un template pour la fonction de fitness
    template <typename FitnessFn>
    neat::Individual run(FitnessFn compute_fitness, int num_generations);
    template <typename FitnessFn>
    neat::Individual runV2(FitnessFn compute_fitness, int num_generations);

    // Méthode pour obtenir les individus
     std::vector<neat::Individual>& get_individuals();

     

private:
    NeatConfig config;
    RNG &rng;
    std::vector<neat::Individual> individuals;  // Vecteur d'individus
    neat::Individual best_individual;

    int next_genome_id;
    int generate_next_genome_id();
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

/**
 * @brief Runs the NEAT algorithm for a specified number of generations.
 *
 * This function simulates the evolution of a population of individuals using the NEAT algorithm.
 * It evaluates the fitness of each individual in the population, updates the best individual,
 * and generates new generations until all individuals reach the goal or the maximum number of generations is reached.
 *
 * @tparam FitnessFn A callable type that computes the fitness of an individual.
 * @param compute_fitness A function that takes an individual's genome and its index, and returns its fitness value.
 * @param max_generations The maximum number of generations to run the simulation.
 * @return The best individual after all generations or when all individuals have reached the goal.
 */
template <typename FitnessFn>
neat::Individual Population::runV2(FitnessFn compute_fitness, int max_generations) {
   /*for (int generation = 0; generation < max_generations; ++generation) {
        bool all_reached_goal = true; // Supposition optimiste au début de chaque génération

        // Calcul de la fitness pour chaque individu
        for (int i = 0; i < individuals.size(); ++i) {
            auto& individual = individuals[i];
            if (!individual.fitness_computed) {
                // Créer un réseau neuronal à partir du génome
                FeedForwardNeuralNetwork nn = create_from_genome(individual.genome);

                // Récupérer l'état du jeu pour cette fourmi
                std::vector<double> game_state = get_game_state(i);

                // Prédire les actions avec le réseau neuronal
                std::vector<double> actions = nn.activate(game_state);

                // Exécuter les actions dans le jeu
                perform_actions(actions, i);

                // Vérifier si la fourmi a atteint la fin du labyrinthe
                if (!has_reached_goal(i)) { 
                    all_reached_goal = false; // Une fourmi n'a pas atteint l'arrivée
                }

                // Évaluer la performance de l'individu
                individual.fitness = compute_fitness(individual.genome, i);
                individual.fitness_computed = true;
            }
        }

        // Met à jour le meilleur individu
        update_best();

        // Si toutes les fourmis ont atteint l'arrivée, on arrête
        if (all_reached_goal) {
            std::cout << "All ants reached the goal in generation " << generation + 1 << std::endl;
            break; // Sort de la boucle car toutes les fourmis ont atteint l'arrivée
        }

        // Générer la nouvelle génération
        individuals = reproduce();

        std::cout << "Generation " << generation + 1 << " completed. Best fitness: " << best_individual.fitness << std::endl;
    }*/

    // Retourne le meilleur individu après toutes les générations
    return best_individual;
}
// TODO : Créer la méthode has_reached_goal pour vérifier si une fourmi a atteint l'arrivée





#endif // POPULATION_H
