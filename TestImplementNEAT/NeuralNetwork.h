#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <vector>
#include <unordered_map>
#include <cassert>
#include <optional>
#include <variant>
#include "Genome.h" // Pour utiliser Genome
#include "ActivationFn.h" // Pour utiliser ActivationFn
#include "neat.h"

struct NeuronInput {
    int input_id;
    double weight;
};

struct Neuron {
    int neuron_id;
    ActivationFn activation;
    double bias;
    std::vector<NeuronInput> inputs;
};

class FeedForwardNeuralNetwork {
public:
     FeedForwardNeuralNetwork(std::vector<int> input_ids, std::vector<int> output_ids, std::vector<Neuron> neurons)
        : m_input_ids(std::move(input_ids)), m_output_ids(std::move(output_ids)), m_neurons(std::move(neurons)) {}

    // Méthode pour activer le réseau avec un ensemble d'entrées
    std::vector<double> activate(const std::vector<double> &inputs);

private:
    std::vector<int> m_input_ids;
    std::vector<int> m_output_ids;
    std::vector<Neuron> m_neurons;
};

ActivationFn convert_activation(const Activation& activation);

// Fonction pour créer un réseau à partir d'un genome
FeedForwardNeuralNetwork create_from_genome(const Genome &genome);
std::vector<std::vector<int>> feed_forward_layer(
    const std::vector<int>& inputs, 
    const std::vector<int>& outputs, 
    const std::vector<neat::LinkGene>& links);

#endif // NEURALNETWORK_H
