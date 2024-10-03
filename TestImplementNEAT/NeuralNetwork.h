#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <vector>
#include <unordered_map>
#include <cassert>
#include <optional>
#include <variant>
#include "Genome.h" // Pour utiliser Genome
#include "ActivationFn.h" // Pour utiliser ActivationFn

struct NeuronInput {
    int input_id;
    double weight;
};

struct Neuron {
    ActivationFn activation;
    double bias;
    std::vector<NeuronInput> inputs;
};

class FeedForwardNeuralNetwork {
public:
    // Méthode pour activer le réseau avec un ensemble d'entrées
    std::vector<double> activate(const std::vector<double> &inputs);

private:
    std::vector<int> m_input_ids;
    std::vector<int> m_output_ids;
    std::vector<Neuron> m_neurons;
};

// Fonction pour créer un réseau à partir d'un genome
FeedForwardNeuralNetwork create_from_genome(const Genome &genome);
std::vector<std::vector<int>> feed_forward_layer(
    const std::vector<int>& inputs, 
    const std::vector<int>& outputs, 
    const std::vector<LinkGene>& links);

#endif // NEURALNETWORK_H
