#ifndef LINK_MUTATOR_H
#define LINK_MUTATOR_H

#include "neat.h"

class LinkMutator {
public:
    // Fonction pour générer un nouveau lien
    neat::LinkGene new_value(int input_id, int output_id) {
        double weight = random_weight();  // Fonction pour obtenir un poids aléatoire
        bool is_enabled = true;  // Le lien est activé par défaut

        neat::LinkId link_id{input_id, output_id};  // Créer un identifiant de lien
        return neat::LinkGene{link_id, weight, is_enabled};  // Retourner le nouveau lien
    }

private:
    double random_weight() {
        // Exemple de génération d'un poids aléatoire entre -1.0 et 1.0
        return ((double)rand() / RAND_MAX) * 2.0 - 1.0; 
    }
};

#endif // LINK_MUTATOR_H
