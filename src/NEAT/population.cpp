#include "Population.h"
#include "Mutator.h"
#include "RNG.h"
#include "ComputeFitness.h"
#include "Neat.h"
#include "Genome.h"
#include <iostream>

Population::Population(NeatConfig config, RNG &rng) 
    : config{config}, rng{rng}, next_genome_id{0} {
    for (int i = 0; i < config.population_size; ++i) {
        int num_hidden_neurons = rng.next_int(1, 4);  // Random hidden neurons
        Genome genome = Genome::create_genome(generate_next_genome_id(), config.num_inputs, config.num_outputs, num_hidden_neurons, rng);
        individuals.push_back(neat::Individual(&genome));
    }
}

std::vector<neat::Individual>& Population::get_individuals() {
    return individuals;
}

int Population::generate_next_genome_id() {
    return next_genome_id++;
}

void Population::mutate(Genome &genome) {
    Mutator::mutate(genome, config, rng);
}

std::vector<neat::Individual> Population::reproduce() {
    auto old_members = sort_individuals_by_fitness(individuals);
    int reproduction_cutoff = std::ceil(config.survival_threshold * old_members.size());
    std::vector<neat::Individual> new_generation;

    std::cout << "Reproducing..." << std::endl;

    while (new_generation.size() < config.population_size) {
        neat::Individual& p1 = rng.choose_random(old_members, reproduction_cutoff);
        neat::Individual& p2 = rng.choose_random(old_members, reproduction_cutoff);

        std::cout << "Crossover between " << p1.genome->get_genome_id() << " and " << p2.genome->get_genome_id() << std::endl;

        neat::Neat neat_instance;
        Genome offspring = neat_instance.crossover(p1.genome, p2.genome, generate_next_genome_id());

        std::cout << "Offspring genome ID: " << offspring.get_genome_id() << std::endl;

        mutate(offspring);

        

        new_generation.push_back(neat::Individual(&offspring));
    }

    return new_generation;
}

std::vector<neat::Individual> Population::sort_individuals_by_fitness(const std::vector<neat::Individual>& individuals) {
    std::vector<neat::Individual> sorted_individuals = individuals;
    std::sort(sorted_individuals.begin(), sorted_individuals.end(), 
        [](const neat::Individual& a, const neat::Individual& b) {
            return a.fitness > b.fitness;
        });
    return sorted_individuals;
}

void Population::update_best() {
    auto best_it = std::max_element(individuals.begin(), individuals.end(), 
        [](const neat::Individual& a, const neat::Individual& b) {
            return a.fitness < b.fitness;
        });
    if (best_it != individuals.end()) {
        best_individual = *best_it;
    }
}
