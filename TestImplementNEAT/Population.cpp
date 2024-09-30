#include "Population.h"
#include "neat.h"
#include "RNG.h"
#include "ComputeFitness.h"
#include "Genome.h"
#include <iostream>

// Constructeur de la classe Population
Population::Population(NeatConfig config, RNG &rng) : config{config}, rng{rng} {
    for (int i = 0; i < config.population_size; ++i) {
        individuals.push_back({new_genome(), kFitnessComputed});
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

// Méthode run pour exécuter les générations et retourner le meilleur individu
neat::Individual Population::run(ComputeFitness &compute_fitness, int num_generations) {
    for (int generation = 0; generation < num_generations; ++generation) {
        // Calculer la fitness de chaque individu
        for (auto &individual : individuals) {
            if (individual.fitness_computed == false) {
                individual.fitness = compute_fitness.evaluate(individual.genome);
                individual.fitness_computed = true;
            }
        }

        // Sélectionner le meilleur individu
        auto best_it = std::max_element(individuals.begin(), individuals.end(), 
            [](const Individual &a, const Individual &b) {
                return a.fitness < b.fitness;
            });

        // Logique de reproduction et mutation pour générer la nouvelle population
        generate_new_population();

        // Afficher des informations sur la génération
        std::cout << "Generation: " << generation << " Best fitness: " << best_it->fitness << std::endl;
    }

    // Retourner le meilleur individu final
    return *std::max_element(individuals.begin(), individuals.end(), 
        [](const Individual &a, const Individual &b) {
            return a.fitness < b.fitness;
        });
}

