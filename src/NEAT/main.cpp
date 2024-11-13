#include <iostream>
#include "Genome.h"
#include "ComputeFitness.h"
#include "Mutator.h"
#include "RNG.h"
#include "Utils.h"
#include "Genome.h"

int main() {
    // Initialiser RNG
    RNG rng;

    // Créer un génome avec des neurones d'entrée, de sortie et des neurones cachés
    int genome_id = 1;
    int num_inputs = 3;
    int num_outputs = 2;
    int num_hidden_neurons = 2;

    Genome genome = Genome::create_genome(genome_id, num_inputs, num_outputs, num_hidden_neurons, rng);

    neat::Individual individual(&genome);

    // Afficher le génome initial
    std::cout << "Génome initial :" << std::endl;
    save(genome, "genome_initial.txt");

    // Tester les mutations
    Mutator mutator;
    std::cout << "\nMutation : ajout d'un neurone" << std::endl;
    mutator.mutate_add_neuron(genome);

    std::cout << "\nMutation : ajout d'un lien" << std::endl;
    mutator.mutate_add_link(genome);

    std::cout << "\nMutation : suppression d'un neurone" << std::endl;
    mutator.mutate_remove_neuron(genome);

    std::cout << "\nGénome après mutations :" << std::endl;
    save(genome, "genome_mutated.txt");

    

    // Calcul de la fitness
    int ant_id = 0;  // ID de la fourmi à tester
    ComputeFitness compute_fitness(rng);
    double fitness = compute_fitness(genome, ant_id);

    std::cout << "\nFitness calculée pour le génome (fourmi " << ant_id << ") : " << fitness << std::endl;

/*
    // Récupérer l'état de jeu d'une fourmi
    std::vector<double> game_state = get_game_state(ant_id);
    std::cout << "\nÉtat de jeu de la fourmi " << ant_id << " :" << std::endl;
    for (size_t i = 0; i < game_state.size(); ++i) {
        std::cout << " - Valeur " << i << " : " << game_state[i] << std::endl;
    }

    // Simuler des actions de la fourmi
    std::vector<double> actions = {1.0, 0.0, 0.5, 0.2};  // Exemple d'actions pour la fourmi
    std::cout << "\nActions exécutées pour la fourmi " << ant_id << ":" << std::endl;
    perform_actions(actions, ant_id);
    */

    return 0;
}
