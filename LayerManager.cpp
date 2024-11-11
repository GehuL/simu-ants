#include "LayerManager.h"
#include <unordered_map>
#include <algorithm>

std::vector<std::vector<int>> LayerManager::organize_layers(
    const std::vector<int>& inputs, 
    const std::vector<int>& outputs, 
    const std::vector<neat::LinkGene>& links) {
    
    std::unordered_set<int> known_neurons(inputs.begin(), inputs.end());
    std::unordered_set<int> output_neurons(outputs.begin(), outputs.end());
    std::vector<std::vector<int>> layers;
    
    layers.push_back(inputs);

    bool added_new_layer = true;
    while (added_new_layer) {
        added_new_layer = false;
        std::vector<int> new_layer;

        for (const auto& link : links) {
            if (known_neurons.count(link.link_id.input_id) && 
                !known_neurons.count(link.link_id.output_id) &&
                !output_neurons.count(link.link_id.output_id)) {
                
                new_layer.push_back(link.link_id.output_id);
                known_neurons.insert(link.link_id.output_id);
                added_new_layer = true;
            }
        }

        if (!new_layer.empty()) {
            layers.push_back(new_layer);
        }
    }

    layers.push_back(outputs);
    return layers;
}

std::vector<int> LayerManager::sort_by_layer(
    const std::vector<int>& layer, 
    const std::vector<neat::LinkGene>& links) {

    std::unordered_map<int, int> neuron_layers;
    for (int neuron : layer) {
        neuron_layers[neuron] = 0;
    }

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& link : links) {
            int input_neuron = link.link_id.input_id;
            int output_neuron = link.link_id.output_id;

            if (neuron_layers.count(input_neuron)) {
                int input_layer = neuron_layers[input_neuron];
                int expected_output_layer = input_layer + 1;

                if (!neuron_layers.count(output_neuron) || neuron_layers[output_neuron] < expected_output_layer) {
                    neuron_layers[output_neuron] = expected_output_layer;
                    changed = true;
                }
            }
        }
    }

    std::vector<int> sorted_layer = layer;
    std::sort(sorted_layer.begin(), sorted_layer.end(), 
              [&](int a, int b) { return neuron_layers[a] < neuron_layers[b]; });

    return sorted_layer;
}
