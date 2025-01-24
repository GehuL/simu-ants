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
    int innovation_number = rng.choose(0.5, a.innovation_number, b.innovation_number);

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

Genome Neat::alt_crossover(const std::shared_ptr<Genome>& dominant, 
                       const std::shared_ptr<Genome>& recessive, 
                       int child_genome_id) {
    Genome offspring{child_genome_id, dominant->get_num_inputs(), dominant->get_num_outputs()};

    //std::cout << "Crossover with shared_ptr" << std::endl;

    // Crossover des neurones
    for (const auto& dominant_neuron : dominant->get_neurons()) {
        int neuron_id = dominant_neuron.neuron_id;
        std::optional<neat::NeuronGene> recessive_neuron = recessive->find_neuron(neuron_id);
        if (!recessive_neuron) {
            offspring.add_neuron(dominant_neuron);
        } else {
            offspring.add_neuron(crossover_neuron(dominant_neuron, *recessive_neuron));
        }
    }

    // Crossover des liens
    for (const auto& dominant_link : dominant->get_links()) {
        LinkId link_id = dominant_link.link_id;
        std::optional<neat::LinkGene> recessive_link = recessive->find_link(link_id);
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
