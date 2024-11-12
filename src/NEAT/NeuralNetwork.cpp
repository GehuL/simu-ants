#include "NeuralNetwork.h"
#include <unordered_set>
#include <iostream>

/**
 * @brief Active le réseau de neurones avec un ensemble d'entrées.
 */
std::vector<double> FeedForwardNeuralNetwork::activate(const std::vector<double>& inputs) {
    assert(inputs.size() == m_input_ids.size());
    std::unordered_map<int, double> values;

    for (size_t i = 0; i < inputs.size(); i++) {
        int input_id = m_input_ids[i];
        values[input_id] = inputs[i];
    }

    for (int output_id : m_output_ids) {
        values[output_id] = 0.0;
    }

    for (const auto& neuron : m_neurons) {
        double value = neuron.bias;

        for (const NeuronInput& input : neuron.inputs) {
            assert(values.find(input.input_id) != values.end());
            value += values[input.input_id] * input.weight;
        }

        value = std::visit([&value](auto&& fn) { return fn(value); }, neuron.activation);
        values[neuron.neuron_id] = value;
    }

    std::vector<double> outputs;
    for (int output_id : m_output_ids) {
        assert(values.find(output_id) != values.end());
        outputs.push_back(values[output_id]);
    }
    return outputs;
}

/**
 * @brief Crée un réseau neuronal à partir d'un génome.
 */
FeedForwardNeuralNetwork FeedForwardNeuralNetwork::create_from_genome(const Genome& genome) {
    std::vector<int> inputs = genome.make_input_ids();
    std::vector<int> outputs = genome.make_output_ids();

    LayerManager layer_manager;
    std::vector<std::vector<int>> layers = layer_manager.organize_layers(inputs, outputs, genome.get_links());

    std::vector<Neuron> neurons;
    for (const auto& layer : layers) {
        std::vector<int> sorted_layer = layer_manager.sort_by_layer(layer, genome.get_links()); 

        for (int neuron_id : sorted_layer) {
            std::vector<NeuronInput> neuron_inputs;

            for (const auto& link : genome.get_links()) {
                if (neuron_id == link.link_id.output_id) {
                    neuron_inputs.emplace_back(NeuronInput{link.link_id.input_id, link.weight});
                }
            }

            auto neuron_gene_opt = genome.find_neuron(neuron_id);
            assert(neuron_gene_opt.has_value());
            const neat::NeuronGene& neuron_gene = *neuron_gene_opt;

            neurons.emplace_back(Neuron{neuron_gene.neuron_id, convert_activation(neuron_gene.activation), neuron_gene.bias, std::move(neuron_inputs)});
        }
    }

    return FeedForwardNeuralNetwork{std::move(inputs), std::move(outputs), std::move(neurons)};
}
