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
            assert(values.find(input.input_id) != values.end());
            value += values[input.input_id] * input.weight;
        }

        value += neuron.bias;
        value = std::visit([&value](auto&& fn) { return fn(value); }, neuron.activation);// Appeler la fonction d'activation

        values[neuron.neuron_id] = value;
    }

    // Récupérer les valeurs de sortie
    std::vector<double> outputs;
    for (int output_id : m_output_ids) {
        assert(values.find(output_id) != values.end());
        outputs.push_back(values[output_id]);
    }
    return outputs;
}

std::vector<std::vector<int>> feed_forward_layer(
    const std::vector<int>& inputs, 
    const std::vector<int>& outputs, 
    const std::vector<neat::LinkGene>& links) {
    
    std::unordered_set<int> known_neurons(inputs.begin(), inputs.end());
    std::vector<std::vector<int>> layers;

    bool added_new_layer = true;
    while (added_new_layer) {
        added_new_layer = false;
        std::vector<int> new_layer;

        for (const auto& link : links) {
            // Utilisation de link.link_id pour accéder à input_id et output_id
            if (known_neurons.count(link.link_id.input_id) && !known_neurons.count(link.link_id.output_id)) {
                new_layer.push_back(link.link_id.output_id);
                known_neurons.insert(link.link_id.output_id);
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

// Fonction utilitaire pour convertir un Activation en ActivationFn
ActivationFn convert_activation(const Activation& activation) {
    switch (activation.get_type()) {
        case Activation::Type::Sigmoid:
            return Sigmoid{};
        case Activation::Type::Tanh:
            return Tanh{};
        default:
            throw std::invalid_argument("Unknown activation type");
    }
}



FeedForwardNeuralNetwork create_from_genome(const Genome &genome) {
    // Utiliser les méthodes adéquates pour obtenir les identifiants d'entrée et de sortie
    std::vector<int> inputs = genome.make_input_ids();
    std::vector<int> outputs = genome.make_output_ids();
    
    // Organiser les neurones en couches
    std::vector<std::vector<int>> layers = feed_forward_layer(inputs, outputs, genome.links);

    std::vector<Neuron> neurons;
    for (const auto &layer : layers) {
        for (int neuron_id : layer) {
            std::vector<NeuronInput> neuron_inputs;
            for (const auto &link : genome.links) {
                // Utiliser link.link_id pour accéder aux identifiants d'entrée et de sortie
                if (neuron_id == link.link_id.output_id) {
                    neuron_inputs.emplace_back(NeuronInput{link.link_id.input_id, link.weight});
                }
            }

            // Trouver le neurone correspondant dans le génome
            auto neuron_gene_opt = genome.find_neuron(neuron_id);
            assert(neuron_gene_opt.has_value());
            const neat::NeuronGene& neuron_gene = *neuron_gene_opt;

            // Créer un neurone en utilisant l'id, la fonction d'activation, le biais, et les entrées
            neurons.emplace_back(Neuron{neuron_gene.neuron_id, convert_activation(neuron_gene.activation), neuron_gene.bias, std::move(neuron_inputs)});
        }
    }

    // Retourner le réseau de neurones créé à partir du génome
    return FeedForwardNeuralNetwork{std::move(inputs), std::move(outputs), std::move(neurons)};
}

