#include "NeuralNetwork.h"
#include <unordered_set>
#include <iostream>

/**
 * @brief Active le réseau de neurones avec un ensemble d'entrées.
 */
std::vector<double> FeedForwardNeuralNetwork::activate(const std::vector<double> &inputs)
{
    // Assurer que le nombre d'entrées correspond
    assert(inputs.size() == m_input_ids.size());

    // Map pour stocker les valeurs des neurones
    std::unordered_map<int, double> values;

    // Initialisation des valeurs d'entrée
    for (size_t i = 0; i < inputs.size(); i++)
    {
        int input_id = m_input_ids[i];
        values[input_id] = inputs[i];
    }

    // Initialiser les neurones cachés et leurs valeurs dans la map values
    for (const auto& neuron : m_neurons)
    {
        if (values.find(neuron.neuron_id) == values.end()) {
            // Initialiser la valeur du neurone avec son biais si elle n'est pas présente
            values[neuron.neuron_id] = neuron.bias;
        }
    }

    // Initialiser les valeurs des neurones de sortie à 0 (ou une autre valeur de départ si nécessaire)
    for (int output_id : m_output_ids)
    {
        values[output_id] = 0.0;
    }

    // Calculer les valeurs des neurones
    for (const auto &neuron : m_neurons)
    {
        double value = neuron.bias;

        for (const NeuronInput &input : neuron.inputs) {
            if (values.find(input.input_id) == values.end()) {
                std::cerr << "Error: input_id " << input.input_id << " not found in values map." << std::endl;
                throw std::runtime_error("Invalid input_id during activation.");
            }
            value += values[input.input_id] * input.weight;
        }

        value = std::visit([&value](auto &&fn)
                           { return fn(value); }, neuron.activation);
        values[neuron.neuron_id] = value;
    }

    // Collecter les sorties du réseau
    std::vector<double> outputs;
    for (int output_id : m_output_ids)
    {
        assert(values.find(output_id) != values.end());
        outputs.push_back(values[output_id]);
    }
    return outputs;
}


/**
 * @brief Crée un réseau neuronal à partir d'un génome.
 */
FeedForwardNeuralNetwork FeedForwardNeuralNetwork::create_from_genome(const Genome &genome)
{
    std::vector<int> inputs = genome.make_input_ids();
    std::vector<int> outputs = genome.make_output_ids();

    assert(!inputs.empty() && "Inputs cannot be empty.");
    assert(!outputs.empty() && "Outputs cannot be empty.");
    assert(!genome.get_links().empty() && "Links cannot be empty.");

    LayerManager layer_manager;
    std::vector<std::vector<int>> layers = layer_manager.organize_layers(inputs, outputs, genome.get_links());

    std::vector<Neuron> neurons;
    for (const auto &layer : layers)
    {
        std::vector<int> sorted_layer = layer_manager.sort_by_layer(layer, genome.get_links());

        for (int neuron_id : sorted_layer)
        {
            std::vector<NeuronInput> neuron_inputs;

            for (const auto &link : genome.get_links())
            {
                if (neuron_id == link.link_id.output_id)
                {
                    neuron_inputs.emplace_back(NeuronInput{link.link_id.input_id, link.weight});
                }
            }

            auto neuron_gene_opt = genome.find_neuron(neuron_id);
            // Vérification : assure qu'un neurone est trouvé dans le génome
            if (!neuron_gene_opt.has_value())
            {
                std::cerr << "Neuron ID " << neuron_id << " not found in genome." << std::endl;
                throw std::runtime_error("Neuron not found.");
            }
            const neat::NeuronGene &neuron_gene = *neuron_gene_opt;

            neurons.emplace_back(Neuron{neuron_gene.neuron_id, convert_activation(neuron_gene.activation), neuron_gene.bias, std::move(neuron_inputs)});
        }
    }

    return FeedForwardNeuralNetwork{std::move(inputs), std::move(outputs), std::move(neurons)};
}

ActivationFn convert_activation(const Activation &activation)
{
    switch (activation.get_type())
    {
    case Activation::Type::Sigmoid:
        return Sigmoid{};
    case Activation::Type::Tanh:
        return Tanh{};
    default:
        throw std::invalid_argument("Unknown activation type");
    }
}
