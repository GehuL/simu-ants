#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "Genome.h"  // Inclure la définition de Genome

// Fonction pour sauvegarder un génome dans un fichier
void save(const Genome &genome, const std::string &filename);

std::vector<double> get_game_state();

void perform_actions(const std::vector<double>& actions);

#endif // UTILS_H
