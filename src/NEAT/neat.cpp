#include "Neat.h"
#include "rng.h" 
#include "GenomeIndexer.h"
#include"Genome.h"  
#include <cassert>
#include <algorithm> 
#include <optional>
#include <unordered_set>
#include <functional>
#include "neuron_mutator.h"
#include <iostream>

namespace neat {

NeuronGene Neat::crossover_neuron(const NeuronGene &a, const NeuronGene &b) {
    assert(a.neuron_id == b.neuron_id);

    RNG rng;  

    int neuron_id = a.neuron_id;
    double bias = rng.choose(0.5, a.bias, b.bias);  // Choix aléatoire du biais
    Activation activation = rng.choose(0.5, a.activation, b.activation);  // Choix aléatoire de l'activation

    return NeuronGene{neuron_id, bias, activation};
}


LinkGene Neat::crossover_link(const LinkGene &a, const LinkGene &b) {
    assert(a.link_id.input_id == b.link_id.input_id);
    assert(a.link_id.output_id == b.link_id.output_id);

    RNG rng;

    LinkId link_id = a.link_id;
    double weight = rng.choose(0.5, a.weight, b.weight);  // Choix aléatoire du poids
    bool is_enabled = rng.choose(0.5, a.is_enabled, b.is_enabled);  // Choix aléatoire de l'activation

    return LinkGene{link_id, weight, is_enabled};
}

Genome Neat::crossover(const Individual &dominant, const Individual &recessive, int child_genome_id) {
    Genome offspring{child_genome_id, dominant.genome->get_num_inputs(), dominant.genome->get_num_outputs()};

    std::cout << "Crossover " << std::endl;

    for (const auto &dominant_neuron : dominant.genome->get_neurons()) {
        int neuron_id = dominant_neuron.neuron_id;
        std::optional<neat::NeuronGene> recessive_neuron = recessive.genome->find_neuron(neuron_id);
        if (!recessive_neuron) {
            offspring.add_neuron(dominant_neuron);
        } else {
            offspring.add_neuron(crossover_neuron(dominant_neuron, *recessive_neuron));
        }
    }

    std::cout << "Crossover2 " << std::endl;

    for (const auto &dominant_link : dominant.genome->get_links()) {
        LinkId link_id = dominant_link.link_id;
        std::optional<neat::LinkGene> recessive_link = recessive.genome->find_link(link_id);
        if (!recessive_link) {
            offspring.add_link(dominant_link);
        } else {
            offspring.add_link(crossover_link(dominant_link, *recessive_link));
        }
    }

    return offspring;
}

double clamp(double x){
    DoubleConfig config;
    return std::min(config.max_value, std::max(config.min_value, x));
}




}
