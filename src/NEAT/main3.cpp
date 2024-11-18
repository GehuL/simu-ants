#include "Population.h"
#include "ComputeFitness.h"
#include "NeuralNetwork.h"
#include "Utils.h"
#include "NeatConfig.h"
#include <iostream>


int main() {
    RNG rng;
    NeatConfig config;
    Population population(config, rng);

    for (const auto &individual : population.get_individuals()) {
    const Genome &genome = *individual.genome;
    std::cout << "Génome ID: " << genome.get_genome_id() << std::endl;

    std::cout << "Neurones:" << std::endl;
    for (const auto &neuron : genome.get_neurons()) {
        std::cout << "  Neuron ID: " << neuron.neuron_id 
                  << ", Biais: " << static_cast<int>(neuron.bias) 
                  << std::endl;
    }

    std::cout << "Liens:" << std::endl;
    for (const auto &link : genome.get_links()) {
        std::cout << "  Link from " << link.link_id.input_id 
                  << " to " << link.link_id.output_id 
                  << " with weight " << link.weight << std::endl;
    }
}

    ComputeFitness compute_fitness(rng);


    const int num_generations = 5;
    const int num_ants = 10;

    for (int generation = 0; generation < num_generations; ++generation) {
        std::cout << "Génération " << generation + 1 << " : " << std::endl;

        // Initialisation des fitness
        for (auto &individual : population.get_individuals()) {
            individual.fitness = 0.0;
        }
           

        // Simulation pour chaque fourmi
        for (int ant_id = 0; ant_id < num_ants; ++ant_id) {
            for (auto &individual : population.get_individuals()) {
                // 1. Créer un réseau neuronal pour cet individu
                FeedForwardNeuralNetwork network = FeedForwardNeuralNetwork::create_from_genome(*individual.genome);

               

                // 2. Obtenir l'état initial de la simulation pour cette fourmi
                std::vector<double> game_state = default_get_game_state(ant_id,rng);

                // 3. Activer le réseau avec l'état de jeu
                std::vector<double> actions = network.activate(game_state);

                // 4. Exécuter les actions dans l'environnement
                default_perform_action(actions,ant_id);

                // 5. Évaluer la fitness de cet individu pour cette fourmi
                individual.fitness += compute_fitness(*individual.genome, ant_id);
            }
        }

        // Sauvegarde des génomes pour suivi
        int individual_index = 0;
        for (const auto &individual : population.get_individuals()) {
            save(*individual.genome, "genome_saves/genome_generation_" 
                                     + std::to_string(generation) 
                                     + "_individual_" 
                                     + std::to_string(individual_index++) + ".txt");
        }

        // Mise à jour du meilleur individu
        population.update_best();

        // Génération de la nouvelle population
        auto new_generation = population.reproduce();
        if (new_generation.empty()) {
            throw std::runtime_error("Erreur : La génération produite est vide !");
        }

        population.replace_population(std::move(new_generation));

        // Affiche la meilleure fitness de la génération
        std::cout << "Meilleure fitness : " << population.get_individuals().front().fitness << std::endl;

        // Sauvegarde du meilleur génome
        save(*population.get_individuals().front().genome, "best_genome_generation_" + std::to_string(generation + 1) + ".txt");
    }

    // Réseau neuronal à partir du meilleur génome
    auto best_genome = population.get_individuals().front().genome;
    FeedForwardNeuralNetwork network = FeedForwardNeuralNetwork::create_from_genome(*best_genome);

    std::vector<double> inputs = { 0.5, 0.3, 0.8 };
    std::vector<double> outputs = network.activate(inputs);

    std::cout << "Sorties du réseau : ";
    for (const auto &output : outputs) {
        std::cout << output << " ";
    }
    std::cout << std::endl;

    return 0;
}
