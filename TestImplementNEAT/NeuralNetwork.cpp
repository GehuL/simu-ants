#include "NeuralNetwork.h"
#include <unordered_set>

std::vector<double> FeedForwardNeuralNetwork::activate(const std::vector<double> &inputs) {
    assert(inputs.size() == m_input_ids.size());
    std::unordered_map<int, double> values;
    
    // Assigner les valeurs d'entrée
    for (int i = 0; i < inputs.size(); i++) {
        int input_id = m_input_ids[i];
        values[input_id] = inputs[i];
    }
    
    // Initialiser les sorties
    for (int output_id : m_output_ids) {
        values[output_id] = 0.0;
    }
    
    // Calcul des valeurs des neurones
    for (const auto &neuron : m_neurons) {
        double value = 0.0;
        for (const NeuronInput& input : neuron.inputs) {
            assert(values.contains(input.input_id));
            value += values[input.input_id] * input.weight;
        }

        value += neuron.bias;
        value = std::visit(neuron.activation, value);  // Activation
        values[neuron.neuron_id] = value;
    }

    // Récupérer les valeurs de sortie
    std::vector<double> outputs;
    for (int output_id : m_output_ids) {
        assert(values.contains(output_id));
        outputs.push_back(values[output_id]);
    }
    return outputs;
}

std::vector<std::vector<int>> feed_forward_layer(
    const std::vector<int>& inputs, 
    const std::vector<int>& outputs, 
    const std::vector<LinkGene>& links) {
    
    std::unordered_set<int> known_neurons(inputs.begin(), inputs.end());
    std::vector<std::vector<int>> layers;

    bool added_new_layer = true;
    while (added_new_layer) {
        added_new_layer = false;
        std::vector<int> new_layer;

        for (const auto& link : links) {
            if (known_neurons.count(link.input_id) && !known_neurons.count(link.output_id)) {
                new_layer.push_back(link.output_id);
                known_neurons.insert(link.output_id);
            }
        }

        if (!new_layer.empty()) {
            layers.push_back(new_layer);
            added_new_layer = true;
        }
    }

    // Ajouter la dernière couche (les neurones de sortie)
    layers.push_back(outputs);
    return layers;
}

FeedForwardNeuralNetwork create_from_genome(const Genome &genome) {
    std::vector<int> inputs = genome.make_input_ids();
    std::vector<int> outputs = genome.make_output_ids();
    
    // Supposons que la fonction feed_forward_layer est définie pour organiser les neurones en couches
    std::vector<std::vector<int>> layers = feed_forward_layer(inputs, outputs, genome.links);

    std::vector<Neuron> neurons;
    for (const auto &layer : layers) {
        for (int neuron_id : layer) {
            std::vector<NeuronInput> neuron_inputs;
            for (const auto &link : genome.links) {
                if (neuron_id == link.output_id) {
                    neuron_inputs.emplace_back(NeuronInput{link.input_id, link.weight});
                }
            }

            auto neuron_gene_opt = genome.find_neuron(neuron_id);
            assert(neuron_gene_opt.has_value());
            neurons.emplace_back(Neuron{*neuron_gene_opt, std::move(neuron_inputs)});
        }
    }
    return FeedForwardNeuralNetwork{std::move(inputs), std::move(outputs), std::move(neurons)};
}
