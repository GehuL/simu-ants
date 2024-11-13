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

#endif // UTILS_H
