#include "Population.h"
#include "ComputeFitness.h"
#include "NeuralNetwork.h"
#include "Utils.h"
#include "NeatConfig.h"
#include <iostream>

int main() {
    RNG rng;  // Générateur de nombres aléatoires
    NeatConfig config;  // Utilise les valeurs par défaut de NeatConfig
    Population population(config, rng);
    ComputeFitness compute_fitness(rng);

    const int num_generations = 5;  // Nombre total de générations
    const int num_ants = 10;          // Nombre d'instances pour évaluation fitness

    for (int generation = 0; generation < num_generations; ++generation) {
        std::cout << "Génération " << generation + 1 << " : " << std::endl;

        // Évalue la fitness de chaque individu de la population
        for (int ant_id = 0; ant_id < num_ants; ++ant_id) {
            for (auto &individual : population.get_individuals()) {
                individual.fitness = compute_fitness(*individual.genome, ant_id);
            }
        }


        
        std::cout<<"Meilleure fitness : "<<population.get_individuals().front().fitness<<std::endl;

        // Met à jour le meilleur individu actuel et trie pour la reproduction
        population.update_best();

        std::cout << "ook " << std::endl;

        
        auto new_generation = population.reproduce();

    
        // Applique les mutations en utilisant les probabilités définies dans NeatConfig
        for (auto &individual : new_generation) {
            population.mutate(*individual.genome);
        }

        // Remplace l'ancienne population par la nouvelle génération
        population.get_individuals() = new_generation;

        // Affiche la meilleure fitness de la génération actuelle
        std::cout << "Meilleure fitness : " << population.get_individuals().front().fitness << std::endl;

        // Sauvegarde le meilleur génome de chaque génération
        save(*population.get_individuals().front().genome, "best_genome_generation_" + std::to_string(generation + 1) + ".txt");
    }

    // Active et teste le réseau du meilleur génome après les générations
    auto best_genome = population.get_individuals().front().genome;
    FeedForwardNeuralNetwork network = FeedForwardNeuralNetwork::create_from_genome(*best_genome);

    // Exemple d'activation du réseau avec des valeurs d'entrée spécifiques
    std::vector<double> inputs = { 0.5, 0.3, 0.8 };
    std::vector<double> outputs = network.activate(inputs);

    // Affiche les sorties du réseau
    std::cout << "Sorties du réseau : ";
    for (const auto &output : outputs) {
        std::cout << output << " ";
    }
    std::cout << std::endl;

    return 0;
}
