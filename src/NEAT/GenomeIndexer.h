#ifndef GENOME_INDEXER_H
#define GENOME_INDEXER_H

class GenomeIndexer {
public:
    GenomeIndexer();  // Constructeur

    int next();  // Méthode pour obtenir le prochain index

private:
    int current_index;  // Compteur pour suivre l'index
};

#endif // GENOME_INDEXER_H
