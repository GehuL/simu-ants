#include "Utils.h"
#include <fstream>
#include <iostream>
#include "neat.h"
#include <random>

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
    game_state.push_back(0.0);  // Exemple de données d'état du jeu

    // Créer un générateur aléatoire
    std::random_device rd;                         // Générateur de nombres aléatoires
    std::mt19937 gen(rd());                        // Mersenne Twister pour une bonne qualité de génération
    std::uniform_real_distribution<> dist(0.0, 1.0); // Distribution aléatoire entre 0.0 et 1.0
    
    // Remplir le vecteur avec des nombres aléatoires
    for (int i = 0; i < 4; ++i) {
        game_state.push_back(dist(gen));
    }

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
        std::cout << "Jump!" << std::endl;
        
    } else {
        std::cout << "Do nothing." << std::endl;
         
    }

    if (actions[1] > 0.5) {
        std::cout << "Cry!" << std::endl;
         
    } else {
        std::cout << "Keep calm." << std::endl;
    }
}

