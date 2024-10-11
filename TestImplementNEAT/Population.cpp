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

// Méthode pour obtenir les individus
std::vector<neat::Individual>& Population::get_individuals() {
    return individuals;
}


// Méthode pour générer le prochain ID de génome
int Population::next_genome_id() {
    static int id = 0;
    return id++;
}

// Méthode pour générer un nouveau génome avec des neurones cachés
Genome Population::new_genome() {
    Genome genome{next_genome_id(), config.num_inputs, config.num_outputs};

    // Ajouter les neurones d'entrée
    for (int neuron_id = 0; neuron_id < config.num_inputs; ++neuron_id) {
        genome.add_neuron(new_neuron(neuron_id));
    }

    // Ajouter les neurones de sortie
    for (int output_id = 0; output_id < config.num_outputs; ++output_id) {
        genome.add_neuron(new_neuron(config.num_inputs + output_id));
    }

    // Ajouter des neurones cachés aléatoirement (entre 1 et 3)
    RNG rng;
    int num_hidden_neurons = rng.next_int(1, 4);  // Génère entre 1 et 3 neurones cachés
    for (int i = 0; i < num_hidden_neurons; ++i) {
        int hidden_id = config.num_inputs + config.num_outputs + i;
        genome.add_neuron(new_neuron(hidden_id));

        // Ajouter des liens entre neurones d'entrée et cachés
        for (int input_id = 0; input_id < config.num_inputs; ++input_id) {
            genome.add_link(new_link(input_id, hidden_id));
        }

        // Ajouter des liens entre neurones cachés et de sortie
        for (int output_id = 0; output_id < config.num_outputs; ++output_id) {
            genome.add_link(new_link(hidden_id, config.num_inputs + output_id));
        }

        // (Facultatif) Ajouter des liens entre neurones cachés (création d'une couche intermédiaire plus complexe)
        for (int j = 0; j < i; ++j) {
            int other_hidden_id = config.num_inputs + config.num_outputs + j;
            genome.add_link(new_link(hidden_id, other_hidden_id));
             // Ajoute les liens bidirectionnels si souhaité
        }
    }

    // Ajouter des liens entre les neurones d'entrée et de sortie
    for (int i = 0; i < config.num_inputs; ++i) {
        for (int output_id = 0; output_id < config.num_outputs; ++output_id) {
            //genome.add_link(new_link(i, config.num_inputs + output_id));
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
    

    while (spawn_size-- > 0) {  // Utilisation de `-- > 0` pour éviter d'ajouter une génération supplémentaire
        RNG rng;
        // Récupérer les parents
        neat::Individual& p1 = rng.choose_random(old_members, reproduction_cutoff);
        neat::Individual& p2 = rng.choose_random(old_members, reproduction_cutoff);
        neat::Neat neat_instance;
        Genome offspring = neat_instance.crossover(p1.genome, p2.genome);  // Vous devez définir `crossover`
        std::cout<<"ok"<<std::endl;
        mutate(offspring);  // Vous devez définir `mutate`
        new_generation.push_back(neat::Individual(offspring));
    }

    return new_generation;
}


std::vector<neat::Individual> Population::sort_individuals_by_fitness(const std::vector<neat::Individual>& individuals) {
    // Créer une copie des individus à trier
    std::vector<neat::Individual> sorted_individuals = individuals;

    // Trie les individus en fonction de leur fitness (du plus grand au plus petit)
    std::sort(sorted_individuals.begin(), sorted_individuals.end(), 
        [](const neat::Individual& a, const neat::Individual& b) {
            return a.fitness > b.fitness;
        });

    return sorted_individuals;
}


void Population::update_best() {
    // Cherche l'individu avec la meilleure fitness
    auto best_it = std::max_element(individuals.begin(), individuals.end(), 
        [](const neat::Individual& a, const neat::Individual& b) {
            return a.fitness < b.fitness;
        });
    
    // Mettre à jour best_individual si un meilleur individu est trouvé
    if (best_it != individuals.end()) {
        best_individual = *best_it;
    }
    
}



