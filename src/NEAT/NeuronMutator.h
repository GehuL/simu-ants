#ifndef NEURON_MUTATOR_H
#define NEURON_MUTATOR_H

#include "neat.h"  // Assurez-vous que neat.h est correctement importé
#include "rng.h"   // Pour utiliser la classe RNG
#include"Activation.h"

namespace neat {

class NeuronMutator {
public:
    NeuronMutator() : next_neuron_id(0) {}

    // Générer un nouveau neurone avec des valeurs par défaut ou aléatoires
    neat::NeuronGene new_neuron() {
        neat::NeuronGene neuron;
        neuron.neuron_id = next_neuron_id++;  // Incrémente l'ID des neurones
        neuron.bias = generate_random_bias();  // Génère un biais aléatoire
        neuron.activation = Activation(Activation::Type::Sigmoid);  // Définit l'activation à Sigmoid
        return neuron;
    }

private:
    int next_neuron_id;

    // Génère un biais aléatoire entre -1 et 1
    double generate_random_bias() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-1.0, 1.0);
        return dis(gen);
    }
};

}  // namespace neat

#endif // NEURON_MUTATOR_H
