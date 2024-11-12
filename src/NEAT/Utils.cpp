#include "Utils.h"
#include <fstream>
#include <iostream>
#include "Neat.h"
#include <random>

/**
 * @brief Sauvegarde un génome dans un fichier.
 * 
 * @param genome Le génome à sauvegarder.
 * @param filename Le nom du fichier dans lequel sauvegarder le génome.
 */
void save(const Genome &genome, const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    // Sauvegarder les informations du génome dans le fichier
    file << "Genome ID: " << genome.get_genome_id() << "\n";
    file << "Neurons:\n";
    for (const auto &neuron : genome.get_neurons()) {
        file << "Neuron ID: " << neuron.neuron_id << ", Bias: " << neuron.bias << "\n";
    }

    file << "Links:\n";
    for (const auto &link : genome.get_links()) {
        file << "Link from neuron " << link.link_id.input_id 
             << " to neuron " << link.link_id.output_id 
             << " with weight " << link.weight 
             << (link.is_enabled ? " (enabled)" : " (disabled)") << "\n";
    }

    file.close();
    std::cout << "Genome saved to " << filename << std::endl;
}

/**
 * @brief Récupère l'état actuel du jeu pour une fourmi spécifique.
 * 
 * @param ant_id L'identifiant de la fourmi.
 * @return Un vecteur contenant la position X, la position Y, l'altitude et la distance à l'objectif.
 */
/*
std::vector<double> get_game_state(int ant_id) {
    std::vector<double> game_state;

    // Récupérer l'état spécifique de la fourmi 'ant_id'
    game_state.push_back(get_ant_position_x(ant_id)); // Position X de la fourmi
    game_state.push_back(get_ant_position_y(ant_id)); // Position Y de la fourmi
    game_state.push_back(get_altitude(ant_id));       // Altitude de la fourmi
    game_state.push_back(get_distance_to_goal(ant_id)); // Distance à l'objectif

    return game_state;
}
*/


/**
 * @brief Effectue une action pour une fourmi en fonction des valeurs fournies.
 * 
 * @param actions Un vecteur de valeurs pour les actions de déplacement (haut, bas, gauche, droite).
 * @param ant_id L'identifiant de la fourmi.
 */
/*
void perform_actions(const std::vector<double>& actions, int ant_id) {
    double move_up = actions[0]; 
    double move_down = actions[1];
    double move_left = actions[2];
    double move_right = actions[3];

    // Choisir l'action avec la plus grande valeur
    if (move_right > move_left && move_right > move_up && move_right > move_down) {
        move_ant_right(ant_id);
    } else if (move_left > move_right && move_left > move_up && move_left > move_down) {
        move_ant_left(ant_id);
    } else if (move_up > move_down && move_up > move_right && move_up > move_left) {
        move_ant_up(ant_id);
    } else if (move_down > move_up && move_down > move_right && move_down > move_left) {
        move_ant_down(ant_id);
    }
}
*/
