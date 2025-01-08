#include "Utils.h"
#include <fstream>
#include <iostream>
#include "Neat.h"
#include <random>
#include <algorithm>

using namespace simu;

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

/**
 * @brief Génère un état de jeu aléatoire pour tester la logique du réseau neuronal.
 * 
 * @param ant_id L'identifiant de la fourmi.
 * @param rng Une référence à un générateur de nombres aléatoires.
 * @return Un vecteur de valeurs représentant l'état du jeu.
 */
std::vector<double> default_get_game_state(int ant_id, RNG &rng) {
    std::vector<double> game_state;

    // Génère des valeurs aléatoires pour l'état du jeu
    double pos_x = rng.next_double();      // Position X entre 0 et 100
    double pos_y = rng.next_double();      // Position Y entre 0 et 100
    double altitude = rng.next_double();    // Altitude entre 0 et 10
    double distance_to_goal = rng.next_double(); // Distance à l'objectif entre 0 et 50

    game_state.push_back(pos_x);
    game_state.push_back(pos_y);
    game_state.push_back(altitude);
    game_state.push_back(distance_to_goal);

    // Affiche l'état du jeu dans le terminal
    std::cout << "Game State for Ant ID " << ant_id << ": ["
              << "X: " << pos_x << ", Y: " << pos_y
              << ", Altitude: " << altitude
              << ", Distance to Goal: " << distance_to_goal
              << "]" << std::endl;

    return game_state;
}

/**
 * @brief Imprime les actions sélectionnées en fonction des sorties du réseau neuronal.
 * 
 * @param actions Un vecteur contenant les valeurs des actions.
 * @param ant_id L'identifiant de la fourmi.
 */
void default_perform_action(const std::vector<double> &actions, int ant_id) {
    // Vérifie que le vecteur d'actions est valide
    if (actions.size() != 4) {
        std::cerr << "Error: Actions vector must contain exactly 4 values!" << std::endl;
        return;
    }

    // Affiche les valeurs des actions
    std::cout << "Actions for Ant ID " << ant_id << ": ["
              << "Up: " << actions[0] << ", Down: " << actions[1]
              << ", Left: " << actions[2] << ", Right: " << actions[3]
              << "]" << std::endl;

    // Trouve l'action avec la plus grande valeur
    auto max_action = std::max_element(actions.begin(), actions.end());
    int action_index = std::distance(actions.begin(), max_action);

    // Détermine et affiche l'action effectuée
    std::string action;
    switch (action_index) {
        case 0: action = "Move Up"; break;
        case 1: action = "Move Down"; break;
        case 2: action = "Move Left"; break;
        case 3: action = "Move Right"; break;
        default: action = "Unknown"; break;
    }

    std::cout << "Selected Action for Ant ID " << ant_id << ": " << action << std::endl;
}

std::vector<double> get_game_state_rpc(int ant_id, RNG &rng) {
    // Exemple simplifié : représenter uniquement le coup précédent de l'adversaire
    int last_opponent_move = rng.next_int(0, 2);  // 0: Rock, 1: Paper, 2: Scissors
    return { double(last_opponent_move) };
}

void perform_action_rpc(const std::vector<double> &actions, int ant_id) {
    int choice = std::distance(actions.begin(), std::max_element(actions.begin(), actions.end()));
    std::cout << "Ant ID " << ant_id << " chooses: " 
              << (choice == 0 ? "Rock" : (choice == 1 ? "Paper" : "Scissors")) 
              << std::endl;
}

std::vector<double> get_game_state_lab(const Vec2i &antPos, const Vec2i &goalPos,  Grid &grid) {
    std::vector<double> state;

    // Position actuelle de la fourmi
    state.push_back(static_cast<double>(antPos.x));
    state.push_back(static_cast<double>(antPos.y));

    // Distance via A* entre la fourmi et l'obj final
    auto path = grid.findPath(antPos, goalPos);  //Je crois que c'est ça la syntaxe
    double path_length = static_cast<double>(path.size()); // Taille du chemin trouvé normalement ?
    state.push_back(path_length);

/*
    // Obstacles environnants (haut, bas, gauche, droite) qu'on peut rajouter en entrée genre la fourmi voit que devant elle y'a un mur ce qui peut peut être retirer une option de mouvemet à force ?
    state.push_back(grid.isWalkable(antPos.x + 1, antPos.y) ? 1.0 : 0.0); // Est
    state.push_back(grid.isWalkable(antPos.x - 1, antPos.y) ? 1.0 : 0.0); // Ouest
    state.push_back(grid.isWalkable(antPos.x, antPos.y + 1) ? 1.0 : 0.0); // Nord
    state.push_back(grid.isWalkable(antPos.x, antPos.y - 1) ? 1.0 : 0.0); // Sud
*/

    return state;
}

void perform_action_lab(const std::vector<double> &actions, simu::AntIA &ant) {
    
    int direction = std::distance(actions.begin(), std::max_element(actions.begin(), actions.end()));

    switch (direction) {
        case 0: ant.move(EAST);  break;
        case 1: ant.move(WEST);  break;
        case 2: ant.move(NORTH); break;
        case 3: ant.move(SOUTH); break;
        default: break;
    }
}
