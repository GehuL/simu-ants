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

Genome Neat::alt_crossover(const std::shared_ptr<Genome>& dominant, 
                       const std::shared_ptr<Genome>& recessive, 
                       int child_genome_id) {
    Genome offspring{child_genome_id, dominant->get_num_inputs(), dominant->get_num_outputs()};

    std::cout << "Crossover with shared_ptr" << std::endl;

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


void to_json(json& json, const neat::LinkGene& link)
{
    json["link_input_id"] = link.link_id.input_id;
    json["link_output_id"] = link.link_id.output_id;
    json["weight"] = link.weight;
    json["is_enabled"] = link.is_enabled;
}

void from_json(const json& json, neat::LinkGene& link)
{
    json["link_input_id"].get_to(link.link_id.input_id);
    json["link_output_id"].get_to(link.link_id.output_id);
    json["weight"].get_to(link.weight);
    json["is_enabled"].get_to(link.is_enabled);
}

void to_json(json& json, const neat::NeuronGene& neuron)
{
    json["neuron_id"] = neuron.neuron_id;
    json["bias"] = neuron.bias;

    switch(neuron.activation.get_type())
    {
        case Activation::Type::Sigmoid:
            json["activation_type"] = "Sigmoid";
            break;
        case Activation::Type::Tanh:
            json["activation_type"] = "Tanh";
            break;
        default:
            json["activation_type"] = "Unknown";
            break;
    }
}

void from_json(const json& json, neat::NeuronGene& neuron)
{
    json["neuron_id"].get_to(neuron.neuron_id);
    json["bias"].get_to(neuron.bias);

    std::string activation_type;
    json["activation_type"].get_to(activation_type);

    if(activation_type == "Sigmoid")
        neuron.activation = Activation(Activation::Type::Sigmoid);
    else if(activation_type == "Tanh")
        neuron.activation = Activation(Activation::Type::Tanh);
    else
        neuron.activation = Activation(Activation::Type::Sigmoid);
}
}
