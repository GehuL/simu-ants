#include "NeuralNetwork.h"
#include <unordered_set>
#include <iostream>

std::vector<double> FeedForwardNeuralNetwork::activate(const std::vector<double>& inputs) {
    std::cout << "Inputs size: " << inputs.size() << "\n";
    std::cout << "m_input_ids size: " << m_input_ids.size() << "\n";
    assert(inputs.size() == m_input_ids.size());

    std::unordered_map<int, double> values;

    // Assigner les valeurs d'entrée
    for (size_t i = 0; i < inputs.size(); i++) {
        int input_id = m_input_ids[i];
        values[input_id] = inputs[i];
    }

    // Initialiser les sorties
    for (int output_id : m_output_ids) {
        values[output_id] = 0.0;
    }

    // Calcul des valeurs des neurones
    for (const auto& neuron : m_neurons) {
        double value = 0.0;

        

        for (const NeuronInput& input : neuron.inputs) {
            std::cout << "Neuron " << neuron.neuron_id << " has input from neuron " << input.input_id << "\n";

            // Vérifier si l'input_id existe dans 'values'
            if (values.find(input.input_id) == values.end()) {
                std::cerr << "Input ID " << input.input_id << " not found in values!" << std::endl;
            }

            // Assertion pour garantir la présence de la valeur dans 'values'
            assert(values.find(input.input_id) != values.end());
            value += values[input.input_id] * input.weight;
        }

        value += neuron.bias;
        value = std::visit([&value](auto&& fn) { return fn(value); }, neuron.activation); // Appeler la fonction d'activation

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
    std::unordered_set<int> output_neurons(outputs.begin(), outputs.end()); // Neurones de sortie
    std::vector<std::vector<int>> layers;

    // Ajouter la première couche (neurones d'entrée)
    layers.push_back(inputs);
    known_neurons.insert(inputs.begin(), inputs.end());

    bool added_new_layer = true;
    while (added_new_layer) {
        added_new_layer = false;
        std::vector<int> new_layer;

        // Parcourir les liens pour ajouter des neurones cachés
        for (const auto& link : links) {
            // Vérifier que le neurone de sortie du lien n'est pas un neurone de sortie
            if (known_neurons.count(link.link_id.input_id) && 
                !known_neurons.count(link.link_id.output_id) &&
                !output_neurons.count(link.link_id.output_id)) { // Exclure les neurones de sortie
                new_layer.push_back(link.link_id.output_id);
                known_neurons.insert(link.link_id.output_id);
            }
        }

        // Ajouter la nouvelle couche s'il y a des neurones à ajouter
        if (!new_layer.empty()) {
            layers.push_back(new_layer);
            added_new_layer = true;
        }
    }

    // Ajouter la dernière couche (neurones de sortie uniquement)
    layers.push_back(outputs);

    for (size_t layer_idx = 0; layer_idx < layers.size(); ++layer_idx) {
    std::cout << "Layer " << layer_idx << ": ";
    for (int neuron_id : layers[layer_idx]) {
        std::cout << neuron_id << " ";
    }
    std::cout << std::endl;
}


    return layers;
}





// Fonction utilitaire pour convertir un Activation en ActivationFn
/**@brief Convertir un Activation en ActivationFn
 * @param activation Activation à convertir
 * @return ActivationFn correspondant à l'Activation
 */
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

    std::cout << "Input IDs: ";
for (int id : inputs) {
    std::cout << id << " ";
}
std::cout << "\nOutput IDs: ";
for (int id : outputs) {
    std::cout << id << " ";
    
}
std::cout << std::endl;
  
    // Organiser les neurones en couches
    std::vector<std::vector<int>> layers = feed_forward_layer(inputs, outputs, genome.links);

    

    std::vector<Neuron> neurons;
    for (const auto& layer : layers) {
    for (int neuron_id : layer) {
        std::vector<NeuronInput> neuron_inputs;
        
        // Vérifier les connexions entrantes
        for (const auto& link : genome.links) {
            if (neuron_id == link.link_id.output_id) {
                neuron_inputs.emplace_back(NeuronInput{link.link_id.input_id, link.weight});
            }
        }

        // Assurer la présence du neurone dans le génome
        auto neuron_gene_opt = genome.find_neuron(neuron_id);
        if (!neuron_gene_opt.has_value()) {
            std::cerr << "Neurone ID " << neuron_id << " non trouve dans le génome." << std::endl;
        }
        assert(neuron_gene_opt.has_value());
        
        const neat::NeuronGene& neuron_gene = *neuron_gene_opt;

        // Ajouter le neurone
        neurons.emplace_back(Neuron{neuron_gene.neuron_id, convert_activation(neuron_gene.activation), neuron_gene.bias, std::move(neuron_inputs)});
    }
}


    // Retourner le réseau de neurones créé à partir du génome
    return FeedForwardNeuralNetwork{std::move(inputs), std::move(outputs), std::move(neurons)};
}

