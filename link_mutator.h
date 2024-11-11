#ifndef LINK_MUTATOR_H
#define LINK_MUTATOR_H

#include "neat.h"
#include "rng.h"

namespace neat
{

    class LinkMutator
    {
    public:
        /**
         * @class LinkMutator
         * @brief Une classe responsable de la mutation des liens dans un réseau de neurones.
         *
         * La classe LinkMutator fournit des fonctionnalités pour muter les liens (connexions)
         * entre les neurones dans un réseau de neurones. Elle utilise un générateur de nombres
         * aléatoires (RNG) pour introduire des variations dans les propriétés des liens.
         *
         * @note Le constructeur par défaut initialise le générateur de nombres aléatoires.
         */
        LinkMutator() : rng() {}

        /**
         * @brief Crée un nouveau LinkGene avec les ID d'entrée et de sortie spécifiés.
         *
         * Cette fonction génère un nouvel objet LinkGene avec un poids aléatoire et
         * le définit comme étant activé par défaut. Le lien est identifié par la combinaison
         * des ID d'entrée et de sortie.
         *
         * @param input_id L'ID du nœud d'entrée.
         * @param output_id L'ID du nœud de sortie.
         * @return LinkGene Le nouvel objet LinkGene créé.
         */
        LinkGene new_link(int input_id, int output_id)
        {
            double weight = generate_random_weight();
            bool is_enabled = true; // Lien activé par défaut
            LinkId link_id{input_id, output_id};
            return LinkGene{link_id, weight, is_enabled};
        }

    private:
        RNG rng;

        /**
         * @brief Génère un poids aléatoire.
         *
         * Cette fonction génère une valeur double aléatoire entre -1.0 et 1.0.
         * Elle utilise la fonction de la bibliothèque standard rand() pour produire un nombre aléatoire,
         * l'échelle à la plage [0, 1], puis le transforme à la plage [-1, 1].
         *
         * @return Une valeur double aléatoire entre -1.0 et 1.0.
         */
        double generate_random_weight()
        {
            return ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        }
    };

} // namespace neat

#endif // LINK_MUTATOR_H
