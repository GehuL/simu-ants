#include "NeuralNetwork.h"
#include <unordered_set>
#include <algorithm>
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
            //std::cout << "Neuron " << neuron.neuron_id << " has input from neuron " << input.input_id << "\n";

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
    
    // Identifie les couches de neurones
    std::vector<std::vector<int>> layers = identify_neuron_layers(inputs, outputs, links);
    
    // Trie les neurones par couches (si nécessaire)
    for (auto& layer : layers) {
        layer = sort_neurons_by_layer(layer, outputs, links);
    }

    std::cout << "Layers: ";
    for (const auto& layer : layers) {
        std::cout << "{ ";
        for (int neuron_id : layer) {
            std::cout << neuron_id << " ";
        }
        std::cout << "} ";
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

std::vector<int> sort_neurons_by_layer(
    const std::vector<int>& inputs,
    const std::vector<int>& outputs,
    const std::vector<neat::LinkGene>& links) {
    
    // Stocker les niveaux des neurones
    std::unordered_map<int, int> neuron_layers;
    
    // Les neurones d'entrée sont dans la première couche
    for (int input_id : inputs) {
        neuron_layers[input_id] = 0;
    }

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& link : links) {
            int input_neuron = link.link_id.input_id;
            int output_neuron = link.link_id.output_id;

            // Si l'entrée est déjà classée, assurez-vous que la sortie est au niveau supérieur
            if (neuron_layers.count(input_neuron)) {
                int input_layer = neuron_layers[input_neuron];
                int expected_output_layer = input_layer + 1;
                
                // Si la sortie n'est pas encore classée ou si elle est à un niveau plus bas que nécessaire
                if (!neuron_layers.count(output_neuron) || neuron_layers[output_neuron] < expected_output_layer) {
                    neuron_layers[output_neuron] = expected_output_layer;
                    changed = true;
                }
            }
        }
    }

    // Collecter et trier les neurones par leur couche
    std::vector<int> sorted_neurons;
    for (const auto& neuron_layer : neuron_layers) {
        sorted_neurons.push_back(neuron_layer.first);
    }
    
    // Optionnel : trier par niveau de neurone (cela dépend de la façon dont tu veux les ordonner)
    std::sort(sorted_neurons.begin(), sorted_neurons.end(), 
              [&](int a, int b) { return neuron_layers[a] < neuron_layers[b]; });
    
    return sorted_neurons;
}

std::vector<std::vector<int>> identify_neuron_layers(
    const std::vector<int>& inputs, 
    const std::vector<int>& outputs, 
    const std::vector<neat::LinkGene>& links) {
    
    // Un ensemble pour garder la trace des neurones déjà connus (traités)
    std::unordered_set<int> known_neurons(inputs.begin(), inputs.end());
    
    // Un autre ensemble pour identifier les neurones de sortie
    std::unordered_set<int> output_neurons(outputs.begin(), outputs.end());

    // Vecteur final des couches
    std::vector<std::vector<int>> layers;

    // La première couche est composée des neurones d'entrée
    layers.push_back(inputs);

    // Parcourir les couches tant qu'on ajoute de nouveaux neurones
    bool added_new_layer = true;
    while (added_new_layer) {
        added_new_layer = false;
        std::vector<int> new_layer;

        // Parcourir les liens pour trouver les nouveaux neurones à ajouter à la prochaine couche
        for (const auto& link : links) {
            // Si le neurone d'entrée de ce lien est déjà connu et que la sortie ne l'est pas encore
            if (known_neurons.count(link.link_id.input_id) && 
                !known_neurons.count(link.link_id.output_id) &&
                !output_neurons.count(link.link_id.output_id)) {
                
                // Ajouter le neurone de sortie à la nouvelle couche
                new_layer.push_back(link.link_id.output_id);
                known_neurons.insert(link.link_id.output_id);
                added_new_layer = true;
            }
        }

        // Si une nouvelle couche a été créée, l'ajouter aux couches
        if (!new_layer.empty()) {
            layers.push_back(new_layer);
        }
    }

    // Ajouter la couche des neurones de sortie à la fin
    layers.push_back(outputs);

    return layers;
}

