#ifndef GENOME_INDEXER_H
#define GENOME_INDEXER_H

class GenomeIndexer
{
public:
    /**
     * @brief Construit un nouvel objet GenomeIndexer et initialise l’index actuel à 0.
     */
    GenomeIndexer(); // Constructeur

    /**
     * @brief Renvoie l’index actuel et l’incrémente.
     *
     * Cette fonction renvoie la valeur actuelle de `current_index` et l’incrémente par un.
     *
     * @return int L’indice courant avant l’incrémentation.
     */
    int next(); // Méthode pour obtenir le prochain index

private:
    int current_index; // Compteur pour suivre l'index
};

#endif // GENOME_INDEXER_H
