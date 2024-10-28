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



//TODO : Créer les méthodes get_ant_position_x, get_ant_position_y, get_altitude, get_distance_to_goal, move_ant_up, move_ant_down, move_ant_left, move_ant_right
std::vector<double> get_game_state(int ant_id) {
    std::vector<double> game_state;

    // Récupérer l'état spécifique de la fourmi 'ant_id'
    game_state.push_back(get_ant_position_x(ant_id)); // Position X de la fourmi
    game_state.push_back(get_ant_position_y(ant_id)); // Position Y de la fourmi
    game_state.push_back(get_altitude(ant_id)); // Altitude de la fourmi
    game_state.push_back(get_distance_to_goal(ant_id)); // Distance à la fin

    return game_state;  // Retourne un vecteur représentant l'état du jeu pour cette fourmi
}

//TODO : Créer les méthodes move_ant_up, move_ant_down, move_ant_left, move_ant_right
void perform_actions(const std::vector<double>& actions, int ant_id) {
    double move_up = actions[0] ; 
    double move_down = actions[1] ;
    double move_left = actions[2] ;
    double move_right = actions[3] ;

    // Choisir l'action avec la plus grande valeur
    if (move_right > move_left && move_right > move_up && move_right > move_down) {
        move_ant_right();
    } else if (move_left > move_right && move_left > move_up && move_left > move_down) {
        move_ant_left();
    } else if (move_up > move_down && move_up > move_right && move_up > move_left) {
        move_ant_up();
    } else if (move_down > move_up && move_down > move_right && move_down > move_left) {
        move_ant_down();
    }
}



