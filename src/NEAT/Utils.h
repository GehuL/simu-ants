#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "Genome.h"  // Inclure la définition de Genome

/**
 * @brief Sauvegarde un génome dans un fichier.
 * 
 * Cette fonction prend un génome et un nom de fichier en paramètres, et sauvegarde
 * les informations du génome dans le fichier spécifié. Les informations sauvegardées
 * incluent l'identifiant du génome, les neurones et les connexions du génome.
 * 
 * @param genome Le génome à sauvegarder.
 * @param filename Le nom du fichier dans lequel sauvegarder le génome.
 * 
 * @note Si le fichier n'existe pas ou ne peut pas être ouvert, un message d'erreur est affiché.
 */
void save(const Genome &genome, const std::string &filename);

/*
std::vector<double> get_game_state(int ant_id);
void perform_actions(const std::vector<double>& actions, int ant_id);
*/

/**
 * @brief Génère un état de jeu aléatoire pour tester la logique du réseau neuronal.
 * 
 * @param ant_id L'identifiant de la fourmi.
 * @param rng Une référence à un générateur de nombres aléatoires.
 * @return Un vecteur de valeurs représentant l'état du jeu.
 */
std::vector<double> default_get_game_state(int ant_id, RNG &rng);

std::vector<double> get_game_state_rpc(int ant_id, RNG &rng);

/**
 * @brief Imprime les actions sélectionnées en fonction des sorties du réseau neuronal.
 * 
 * @param actions Un vecteur contenant les valeurs des actions.
 * @param ant_id L'identifiant de la fourmi.
 */
void default_perform_action(const std::vector<double> &actions, int ant_id);

void perform_action_rpc(const std::vector<double> &actions, int ant_id);

std::vector<double> get_game_state_lab(const Vec2i &antPos, const Vec2i &goalPos, const Grid &grid);

void perform_action_lab(const std::vector<double> &actions, Ant &ant);

#endif // UTILS_H
