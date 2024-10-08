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
    Population(NeatConfig config, RNG &rng);

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

template <typename FitnessFn>
neat::Individual Population::runV2(FitnessFn compute_fitness, int num_generations) {
    for (int generation = 0; generation < num_generations; ++generation) {
        // Calcul de la fitness pour chaque individu
        for (auto &individual : individuals) {
            if (!individual.fitness_computed) {
                // Sauvegarde du génome avant de créer le réseau neuronal
                std::string filename = "genome_saves/genome_before_create_from_genome_generation_" 
                                       + std::to_string(generation) 
                                       + "_individual_" 
                                       + std::to_string(&individual - &individuals[0]) 
                                       + ".txt";
                
                save(individual.genome, filename);  // Sauvegarde dans le dossier 'genomes_saves'
                
                // Créer un réseau neuronal à partir du génome
                FeedForwardNeuralNetwork nn = create_from_genome(individual.genome);

                // Récupérez l'état du jeu
                std::vector<double> game_state = get_game_state();

                // Prédisez les actions avec le réseau neuronal
                std::vector<double> actions = nn.activate(game_state);

                // Exécutez les actions dans le jeu
                perform_actions(actions);

                // Évaluez la performance de l'individu
                individual.fitness = compute_fitness(individual.genome);
                individual.fitness_computed = true;
            }
        }

        // Met à jour le meilleur individu
        update_best();

        // Génère la nouvelle génération
        individuals = reproduce();

        std::cout << "Generation " << generation + 1 << " completed. Best fitness: " << best_individual.fitness << std::endl;
    }

    // Retourne le meilleur individu après toutes les générations
    return best_individual;
}


#endif // POPULATION_H
