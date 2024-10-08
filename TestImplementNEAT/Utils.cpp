#include "Utils.h"
#include <fstream>
#include <iostream>
#include "neat.h"

// Implémentation de la fonction save
void save(const Genome &genome, const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    // Sauvegarder les informations du génome dans le fichier
    file << "Genome ID: " << genome.get_genome_id() << "\n";
    file << "Neurons:\n";
    for (const auto &neuron : genome.neurons) {
        file << "Neuron ID: " << neuron.neuron_id << ", Bias: " << neuron.bias << "\n";
    }

    file << "Links:\n";
    for (const auto &link : genome.links) {
        file << "Link from neuron " << link.link_id.input_id 
             << " to neuron " << link.link_id.output_id 
             << " with weight " << link.weight 
             << (link.is_enabled ? " (enabled)" : " (disabled)") << "\n";
    }

    file.close();
    std::cout << "Genome saved to " << filename << std::endl;
}

std::vector<double> get_game_state() {
    std::vector<double> game_state;

    // Exemple d'état du jeu, vous devez adapter cela à votre jeu spécifique
    //game_state.push_back(player_position_x);
    //game_state.push_back(player_position_y);
    //game_state.push_back(enemy_distance);
    //game_state.push_back(player_health);

    return game_state;  // Retourne un vecteur représentant l'état du jeu
}

void perform_actions(const std::vector<double>& actions) {
    // Exemple d'actions à partir des sorties du réseau de neurones
    if (actions[0] > 0.5) {
        
    } else {
         
    }

    if (actions[1] > 0.5) {
         
    } else {
        
    }
}

