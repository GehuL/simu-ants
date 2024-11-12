#include "Mutator.h"
#include "Genome.h"
#include"neuron_mutator.h"
#include "link_mutator.h"
#include "RNG.h"
#include <iostream>
#include <functional>


void Mutator::mutate(Genome &genome, const NeatConfig &config, RNG &rng) {
    if (rng.next_double() < config.probability_add_link) {
        mutate_add_link(genome);
    }
    if (rng.next_double() < config.probability_remove_link) {
        mutate_remove_link(genome);
    }
    if (rng.next_double() < config.probability_add_neuron) {
        mutate_add_neuron(genome);
    }
    if (rng.next_double() < config.probability_remove_neuron) {
        mutate_remove_neuron(genome);
    }

    // Mutate weights and biases
    if (rng.next_bool()) {
        mutate_link_weight(genome, config, rng);
    } else {
        mutate_neuron_bias(genome, config, rng);
    }
}

void Mutator::mutate_add_link(Genome &genome) { 
    int input_id = choose_random_input_or_hidden_neuron(genome.get_neurons());  
    int output_id = choose_random_output_or_hidden_neuron(genome.get_neurons());

    if (input_id == -1 || output_id == -1) {
        return;
    }

    neat::LinkId link_id{input_id, output_id};

    auto existing_link = genome.find_link(link_id);
    if (existing_link) {
        if (!existing_link->is_enabled) {
            existing_link->is_enabled = true;
        }
        return;
    }

    if (would_create_cycle(genome.get_links(), input_id, output_id)) {
        return;
    }

    neat::LinkMutator link_mutator;
    neat::LinkGene new_link = link_mutator.new_value(input_id, output_id);
    genome.add_link(new_link);

}

void Mutator::mutate_remove_link(Genome &genome) {
    RNG rng;
    NeatConfig config;

    if (genome.get_links().empty()) {
        return;
    }

    std::unordered_set<neat::LinkId, neat::LinkIdHash> essential_links;

    for (const auto& neuron : genome.get_neurons()) {
        if (neuron.neuron_id < config.num_inputs) {
            for (const auto& link : genome.get_links()) {
                if (link.link_id.input_id == neuron.neuron_id) {
                    essential_links.insert(link.link_id);
                }
            }
        }
    }

    for (const auto& neuron : genome.get_neurons()) {
        if (neuron.neuron_id >= config.num_inputs && 
            neuron.neuron_id < config.num_inputs + config.num_outputs) {
            for (const auto& link : genome.get_links()) {
                if (link.link_id.output_id == neuron.neuron_id) {
                    essential_links.insert(link.link_id);
                }
            }
        }
    }

    for (const auto& link : genome.get_links()) {
        if (link.link_id.input_id >= config.num_inputs && link.link_id.output_id >= config.num_inputs) {
            essential_links.insert(link.link_id);
        }
    }

    std::vector<neat::LinkGene> removable_links;
    for (const auto& link : genome.get_links()) {
        if (essential_links.find(link.link_id) == essential_links.end()) {
            removable_links.push_back(link);
        }
    }

    if (removable_links.empty()) {
        return;
    }

    auto to_remove = rng.choose_random(removable_links);
    genome.get_links().erase(std::remove(genome.get_links().begin(), genome.get_links().end(), to_remove), genome.get_links().end());
}

void Mutator::mutate_add_neuron(Genome &genome) {
    RNG rng;

    if (genome.get_links().empty()) {
        return;
    }

    neat::LinkGene link_to_split = rng.choose_random(genome.get_links());
    link_to_split.is_enabled = false;

    genome.get_links().erase(std::remove_if(genome.get_links().begin(), genome.get_links().end(),
        [&link_to_split](const neat::LinkGene &link) {
            return link.link_id == link_to_split.link_id;
        }), genome.get_links().end());

    neat::NeuronMutator neuron_mutator;
    neat::NeuronGene new_neuron = neuron_mutator.new_neuron();
    new_neuron.neuron_id = genome.generate_next_neuron_id();
    genome.add_neuron(new_neuron);

    neat::LinkId link_id = link_to_split.link_id;
    double weight = link_to_split.weight;

    genome.add_link(neat::LinkGene{{link_id.input_id, new_neuron.neuron_id}, 1.0, true});
    genome.add_link(neat::LinkGene{{new_neuron.neuron_id, link_id.output_id}, weight, true});
}

void Mutator::mutate_remove_neuron(Genome &genome) {
    int hidden_neuron_count = std::count_if(genome.get_neurons().begin(), genome.get_neurons().end(), 
        [](const neat::NeuronGene &neuron) { 
            NeatConfig config;
            return neuron.is_hidden(neuron.neuron_id, config);
        });

    if (hidden_neuron_count < 2) {
        return;
    }

    RNG rng;
    auto neuron_it = choose_random_hidden(genome.get_neurons());

    genome.get_links().erase(std::remove_if(genome.get_links().begin(), genome.get_links().end(), 
        [neuron_it](const neat::LinkGene &link) {
            return link.has_neuron(*neuron_it);
        }),
        genome.get_links().end());

    genome.get_neurons().erase(neuron_it);
}

void Mutator::mutate_link_weight(Genome &genome, const NeatConfig &config, RNG &rng) {
    // Vérifie s'il y a des liens à muter
    if (genome.get_links().empty()) {
        return;
    }

    // Choisir un lien aléatoire
    int link_index = rng.next_int(0, genome.get_links().size() - 1);
    auto &link = genome.get_links()[link_index];

    // Appliquer la mutation si la probabilité le permet
    if (rng.next_double() < config.probability_mutate_link_weight) {
        std::cout << "Mutating link weight for genome " << genome.get_genome_id() << std::endl;
        link.weight = mutate_delta(link.weight);  // Muter le poids du lien
    }
}

void Mutator::mutate_neuron_bias(Genome &genome, const NeatConfig &config, RNG &rng) {
    // Vérifie s'il y a des neurones à muter
    if (genome.get_neurons().empty()) {
        return;
    }

    // Choisir un neurone aléatoire
    int neuron_index = rng.next_int(0, genome.get_neurons().size() - 1);
    auto &neuron = genome.get_neurons()[neuron_index];

    // Appliquer la mutation si la probabilité le permet
    if (rng.next_double() < config.probability_mutate_neuron_bias) {
        std::cout << "Mutating neuron bias for genome " << genome.get_genome_id() << std::endl;
        neuron.bias = mutate_delta(neuron.bias);  // Muter le biais du neurone
    }
}



int choose_random_input_or_hidden_neuron(const std::vector<neat::NeuronGene>& neurons) {
    std::vector<int> valid_neurons;
    NeatConfig config;

    for (const auto& neuron : neurons) {
        if (neuron.neuron_id < config.num_inputs || 
            neuron.neuron_id >= config.num_inputs + config.num_outputs) {
            valid_neurons.push_back(neuron.neuron_id);
        }
    }

    if (valid_neurons.empty()) {
        return -1;
    }

    int random_index = std::rand() % valid_neurons.size();
    return valid_neurons[random_index];
}

int choose_random_output_or_hidden_neuron(const std::vector<neat::NeuronGene>& neurons) {
    std::vector<int> valid_neurons;
    NeatConfig config;

    for (const auto& neuron : neurons) {
        if (neuron.neuron_id >= config.num_inputs && 
            neuron.neuron_id < config.num_inputs + config.num_outputs) {
            valid_neurons.push_back(neuron.neuron_id);
        }
    }
    if (valid_neurons.empty()) {
        return -1;
    }
    int random_index = std::rand() % valid_neurons.size();
    return valid_neurons[random_index];
}

std::vector<neat::NeuronGene>::const_iterator choose_random_hidden(std::vector<neat::NeuronGene>& neurons) {
    std::vector<std::vector<neat::NeuronGene>::const_iterator> hidden_neurons;
    NeatConfig config;

    for (auto it = neurons.begin(); it != neurons.end(); ++it) {
        if (it->neuron_id >= config.num_inputs + config.num_outputs) {
            hidden_neurons.push_back(it);
        }
    }

    if (hidden_neurons.empty()) {
        throw std::out_of_range("No hidden neurons available.");
    }

    RNG rng;
    return rng.choose_random(hidden_neurons);
}



bool would_create_cycle(const std::vector<neat::LinkGene>& links, int input_id, int output_id) {
    std::unordered_set<int> visited;

    std::function<bool(int)> dfs = [&](int current_id) {
        if (visited.find(current_id) != visited.end()) {
            return true;
        }
        visited.insert(current_id);

        for (const auto& link : links) {
            if (link.link_id.input_id == current_id) {
                if (link.link_id.output_id == input_id) {
                    return true;
                }
                if (dfs(link.link_id.output_id)) {
                    return true;
                }
            }
        }
        return false;
    };

    return dfs(output_id);
}

double new_value(){
    RNG rng;
    neat::DoubleConfig config;
    return neat::clamp(rng.next_gaussian(config.init_mean, config.init_stdev));
}

double mutate_delta(double value){
    RNG rng;
    neat::DoubleConfig config;
    double delta = neat::clamp( rng.next_gaussian(0, config.mutate_power));
    return neat::clamp (value + delta);
}

