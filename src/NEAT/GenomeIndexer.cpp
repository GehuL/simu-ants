#include "GenomeIndexer.h"

/**
 * @brief Construit un nouvel objet GenomeIndexer et initialise l’index actuel à 0.
 */
GenomeIndexer::GenomeIndexer() : current_index(0) {}

/**
 * @brief Renvoie l’index actuel et l’incrémente.
 * 
 * Cette fonction renvoie la valeur actuelle de `current_index` et l’incrémente par un.
 * 
 * @return int L’indice courant avant l’incrémentation.
 */
int GenomeIndexer::next() {
    return current_index++;  // Renvoie l'index actuel et l'incrémente
}
