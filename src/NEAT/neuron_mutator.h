#ifndef NEURON_MUTATOR_H
#define NEURON_MUTATOR_H

#include "Neat.h"
#include "rng.h"
#include "Activation.h"

namespace neat
{

    class NeuronMutator
    {
    public:
        /**
         * @class NeuronMutator
         * @brief Une classe responsable de la mutation des neurones dans un réseau de neurones.
         *
         * La classe NeuronMutator fournit des fonctionnalités pour muter les neurones en leur attribuant
         * des ID uniques et en utilisant un générateur de nombres aléatoires pour les opérations de mutation.
         *
         * @constructor
         * Initialise le NeuronMutator avec un ID de neurone de départ à 0 et un générateur
         * de nombres aléatoires par défaut.
         */
        NeuronMutator() : next_neuron_id(0), rng() {}

        /**
         * @brief Crée un nouveau neurone avec un ID unique et un biais aléatoire.
         *
         * Cette méthode génère un nouveau neurone avec un ID unique et une valeur de biais aléatoire
         * entre -1 et 1. Le neurone est initialisé avec une fonction d'activation par défaut
         * de type Sigmoid.
         *
         * @return NeuronGene Une structure NeuronGene représentant le neurone nouvellement créé.
         */
        NeuronGene new_neuron()
        {
            NeuronGene neuron;
            neuron.neuron_id = next_neuron_id++;                       // Incrémente l'ID des neurones
            neuron.bias = generate_random_bias();                      // Génère un biais aléatoire
            neuron.activation = Activation(Activation::Type::Sigmoid); // Activation par défaut: Sigmoid
            return neuron;
        }

    private:
        int next_neuron_id;
        RNG rng;

        /**
         * @brief Génère une valeur de biais aléatoire.
         *
         * Cette fonction génère une valeur de biais aléatoire entre -1.0 et 1.0 en utilisant une
         * distribution uniforme réelle. Elle utilise le random_device pour initialiser le
         * générateur de nombres aléatoires Mersenne Twister.
         *
         * @return Un double représentant la valeur de biais aléatoire générée.
         */
        double generate_random_bias()
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dis(-1.0, 1.0);
            return dis(gen);
        }
    };

} // namespace neat

#endif // NEURON_MUTATOR_H
