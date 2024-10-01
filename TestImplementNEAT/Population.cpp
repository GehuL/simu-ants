#include "Population.h"
#include "neat.h"
#include "RNG.h"
#include "ComputeFitness.h"
#include "Genome.h"
#include <iostream>
#include <algorithm>  // Pour std::max_element

// Constructeur de la classe Population
Population::Population(NeatConfig config, RNG &rng) : config{config}, rng{rng} {
    for (int i = 0; i < config.population_size; ++i) {
        // Utiliser Individual au lieu de std::pair<Genome, bool>
        individuals.push_back(neat::Individual(new_genome()));
    }
}


// Méthode pour générer le prochain ID de génome
int Population::next_genome_id() {
    static int id = 0;
    return id++;
}

// Méthode pour générer un nouveau génome
Genome Population::new_genome() {
    Genome genome{next_genome_id(), config.num_inputs, config.num_outputs};

    // Ajouter les neurones d'entrée
    for (int neuron_id = 0; neuron_id < config.num_inputs; ++neuron_id) {
        genome.add_neuron(new_neuron(neuron_id));
    }

    // Ajouter les liens entre les neurones d'entrée et de sortie
    for (int i = 0; i < config.num_inputs; i++) {
        int input_id = -i - 1;
        for (int output_id = 0; output_id < config.num_outputs; ++output_id) {
            genome.add_link(new_link(input_id, output_id));
        }
    }

    return genome;
}

// Méthode pour créer un nouveau neurone
neat::NeuronGene Population::new_neuron(int neuron_id) {
    neat::NeuronGene neuron;
    neuron.neuron_id = neuron_id;
    neuron.bias = 0.0;
    neuron.activation = Activation(Activation::Type::Sigmoid);  // Activation Sigmoid par défaut
    return neuron;
}

// Méthode pour créer un nouveau lien
neat::LinkGene Population::new_link(int input_id, int output_id) {
    neat::LinkGene link;
    link.link_id = {input_id, output_id};
    link.weight = rng.next_gaussian(0.0, 1.0);  // Poids aléatoire
    link.is_enabled = true;
    return link;
}

void Population::mutate(Genome &genome) {
    RNG rng;
    
    
    // Probabilité d'ajouter un lien
    if (rng.next_double() < config.probability_add_link) {
        neat::mutate_add_link(genome);
    }

    // Probabilité de supprimer un lien
    if (rng.next_double() < config.probability_remove_link) {
        neat::mutate_remove_link(genome);
    }

    // Probabilité d'ajouter un neurone
    if (rng.next_double() < config.probability_add_neuron) {
        neat::mutate_add_neuron(genome);
    }

    // Probabilité de supprimer un neurone
    if (rng.next_double() < config.probability_remove_neuron) {
        neat::mutate_remove_neuron(genome);
    }

    // Appliquer la mutation non structurelle aux poids des liens
    for (auto &link : genome.links) {
        if (rng.next_double() < config.probability_mutate_link_weight) {
            link.weight = neat::mutate_delta(link.weight);  // Muter le poids du lien
        }
    }

    // Appliquer la mutation non structurelle aux biais des neurones
    for (auto &neuron : genome.neurons) {
        if (rng.next_double() < config.probability_mutate_neuron_bias) {
            neuron.bias = neat::mutate_delta(neuron.bias);  // Muter le biais du neurone
        }
    }
}



// Exemple de la méthode reproduce() que vous pourriez implémenter
std::vector<neat::Individual> Population::reproduce() {
    auto old_members = sort_individuals_by_fitness(individuals);
    int reproduction_cutoff = std::ceil(config.survival_threshold * old_members.size());
    std::vector<neat::Individual> new_generation;
    int spawn_size = config.population_size;

    while (spawn_size-- >= 0) {
        RNG rng;
        const auto& p1 = *rng.choose_random(old_members, reproduction_cutoff);
        const auto& p2 = *rng.choose_random(old_members, reproduction_cutoff);
        neat::Neat neat_instance;
        Genome offspring = neat_instance.crossover(p1.genome, p2.genome);  // Vous devez définir `crossover`
        mutate(offspring);  // Vous devez définir `mutate`
        new_generation.push_back(neat::Individual(offspring));
    }

    return new_generation;
}


