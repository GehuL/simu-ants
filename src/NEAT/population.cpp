#include "Population.h"
#include "Mutator.h"
#include "RNG.h"
#include "ComputeFitness.h"
#include "Neat.h"
#include "Genome.h"
#include <iostream>
#include <memory>



Population::Population(NeatConfig config, RNG &rng) 
    : config{config}, rng{rng}, next_genome_id{0} {
    for (int i = 0; i < config.population_size; ++i) {
        int num_hidden_neurons = rng.next_int(1, 4);  // Random hidden neurons
std::shared_ptr<Genome> genome = std::make_shared<Genome>(Genome::create_genome(generate_next_genome_id(), config.num_inputs, config.num_outputs, num_hidden_neurons, rng));
individuals.emplace_back(genome);

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

        

        new_generation.push_back(neat::Individual(std::make_shared<Genome>(offspring)));

    }

    return new_generation;
}




std::vector<neat::Individual> Population::reproduce_from_genomes(const std::vector<std::shared_ptr<Genome>>& genomes) {
    if (genomes.empty()) {
        throw std::runtime_error("Erreur : La liste de génomes est vide. Impossible de reproduire.");
    }

    // Initialiser une instance de ComputeFitness
    ComputeFitness compute_fitness(rng);

    // Trier les génomes par fitness évaluée (sans stocker la fitness dans les objets Genome)
    std::vector<std::shared_ptr<Genome>> sorted_genomes = genomes;
    std::sort(sorted_genomes.begin(), sorted_genomes.end(),
        [&compute_fitness](const std::shared_ptr<Genome>& a, const std::shared_ptr<Genome>& b) {
            // Évaluer la fitness pour comparer les génomes
            return compute_fitness(*a, /* ant_id */ 0) > compute_fitness(*b, /* ant_id */ 0);
        });

    int reproduction_cutoff = std::ceil(config.survival_threshold * sorted_genomes.size());
    std::vector<neat::Individual> new_generation;

    std::cout << "Reproducing from custom genome list..." << std::endl;

    // Boucle pour créer la nouvelle génération
    while (new_generation.size() < config.population_size) {
        const std::shared_ptr<Genome>& p1 = rng.choose_random(sorted_genomes, reproduction_cutoff);
        const std::shared_ptr<Genome>& p2 = rng.choose_random(sorted_genomes, reproduction_cutoff);

        std::cout << "Crossover between " << p1->get_genome_id() << " and " << p2->get_genome_id() << std::endl;

        neat::Neat neat_instance;
        Genome offspring_genome = neat_instance.alt_crossover(p1, p2, generate_next_genome_id());
        std::shared_ptr<Genome> offspring = std::make_shared<Genome>(offspring_genome);

        std::cout << "Offspring genome ID: " << offspring->get_genome_id() << std::endl;

        mutate(*offspring);

        new_generation.push_back(neat::Individual(offspring));
    }

    return new_generation;
}

std::vector<neat::Individual> Population::reproduce_from_genomes_with_fitness(
    const std::vector<std::shared_ptr<Genome>>& genomes,
    const std::vector<double>& fitnesses
) {
    if (genomes.empty() || fitnesses.empty() || genomes.size() != fitnesses.size()) {
        throw std::runtime_error("Erreur : Liste de génomes ou de fitness invalide.");
    }

    // Trier les génomes par fitness (en tenant compte de la configuration de seuil de survie)
    std::vector<std::shared_ptr<Genome>> sorted_genomes = genomes;
    std::vector<double> sorted_fitnesses = fitnesses;

    // Tri des génomes et des fitness associés (du meilleur au moins bon)
    std::vector<std::pair<std::shared_ptr<Genome>, double>> genome_fitness_pairs;
    for (size_t i = 0; i < genomes.size(); ++i) {
        genome_fitness_pairs.push_back({genomes[i], fitnesses[i]});
    }

    std::sort(genome_fitness_pairs.begin(), genome_fitness_pairs.end(),
        [](const std::pair<std::shared_ptr<Genome>, double>& a, const std::pair<std::shared_ptr<Genome>, double>& b) {
            return a.second > b.second;  // Tri décroissant selon la fitness
        });

    // Récupérer les génomes triés et les fitness correspondants
    for (size_t i = 0; i < genome_fitness_pairs.size(); ++i) {
        sorted_genomes[i] = genome_fitness_pairs[i].first;
        sorted_fitnesses[i] = genome_fitness_pairs[i].second;
    }

    int reproduction_cutoff = std::ceil(config.survival_threshold * sorted_genomes.size());
    std::vector<neat::Individual> new_generation;

    std::cout << "Reproducing from sorted genome list with fitness..." << std::endl;

    // Boucle pour créer la nouvelle génération
    while (new_generation.size() < config.population_size) {
        // Sélectionner deux parents parmi les meilleurs génomes (selon le seuil de survie)
        const std::shared_ptr<Genome>& p1 = rng.choose_random(sorted_genomes, reproduction_cutoff);
        const std::shared_ptr<Genome>& p2 = rng.choose_random(sorted_genomes, reproduction_cutoff);

        std::cout << "Crossover between " << p1->get_genome_id() << " and " << p2->get_genome_id() << std::endl;

        // Crossover
        neat::Neat neat_instance;
        Genome offspring_genome = neat_instance.alt_crossover(p1, p2, generate_next_genome_id());
        std::shared_ptr<Genome> offspring = std::make_shared<Genome>(offspring_genome);

        // Mutation
        mutate(*offspring);

        // Ajouter à la nouvelle génération
        new_generation.push_back(neat::Individual(offspring));
    }

    return new_generation;
}



std::vector<neat::Individual> Population::reproduce_from_genome_roulette(
    const std::vector<std::shared_ptr<Genome>>& genomes,
    const std::vector<double>& fitnesses
) {
    if (genomes.empty() || fitnesses.empty() || genomes.size() != fitnesses.size()) {
        throw std::runtime_error("Erreur : Liste de génomes ou de fitness invalide.");
    }

    std::vector<neat::Individual> new_generation;

    while (new_generation.size() < config.population_size) {
        // Sélection des parents par roulette
        const auto& p1 = rng.roulette_selection(genomes, fitnesses);
        const auto& p2 = rng.roulette_selection(genomes, fitnesses);

        // Crossover
        neat::Neat neat_instance;
        Genome offspring_genome = neat_instance.alt_crossover(p1, p2, generate_next_genome_id());
        std::shared_ptr<Genome> offspring = std::make_shared<Genome>(offspring_genome);

        // Mutation
        mutate(*offspring);

        // Ajouter à la nouvelle génération
        new_generation.push_back(neat::Individual(offspring));
    }

    return new_generation;
}

std::vector<neat::Individual> Population::reproduce_from_genome_roulette_negative(
    const std::vector<std::shared_ptr<Genome>>& genomes,
    const std::vector<double>& fitnesses
) {
    if (genomes.empty() || fitnesses.empty() || genomes.size() != fitnesses.size()) {
        throw std::runtime_error("Erreur : Liste de génomes ou de fitness invalide.");
    }

    // Étape 1 : Ajuster les fitness pour les rendre positives
    double min_fitness = *std::min_element(fitnesses.begin(), fitnesses.end());
    std::vector<double> adjusted_fitnesses = fitnesses;

    if (min_fitness < 0) {
        for (double& fitness : adjusted_fitnesses) {
            fitness += std::abs(min_fitness) + 1.0; // Translation pour rendre toutes les fitness positives
        }
    }

    // Étape 2 : Création de la nouvelle génération
    std::vector<neat::Individual> new_generation;

    while (new_generation.size() < config.population_size) {
        // Sélection des parents par roulette biaisée sur les fitness ajustées
        const auto& p1 = rng.roulette_selection(genomes, adjusted_fitnesses);
        const auto& p2 = rng.roulette_selection(genomes, adjusted_fitnesses);

        // Crossover
        neat::Neat neat_instance;
        Genome offspring_genome = neat_instance.alt_crossover(p1, p2, generate_next_genome_id());
        std::shared_ptr<Genome> offspring = std::make_shared<Genome>(offspring_genome);

        // Mutation
        mutate(*offspring);

        // Ajouter à la nouvelle génération
        new_generation.push_back(neat::Individual(offspring));
    }

    return new_generation;
}

std::vector<neat::Individual> Population::reproduce_with_speciation(
    const std::vector<Species>& species_list,
    const std::unordered_map<std::shared_ptr<Genome>, double>& fitness_map
) {
    std::vector<neat::Individual> new_generation;

    for (const auto &species : species_list) {
        // Ajuster les fitness de l'espèce
        std::vector<double> adjusted_fitnesses;
        double min_fitness = std::numeric_limits<double>::max();  

        for (const auto &genome : species.members) {
            if (fitness_map.find(genome) != fitness_map.end()) {
    double adjusted_fitness = fitness_map.at(genome) / species.members.size();
    
    adjusted_fitnesses.push_back(adjusted_fitness);

            // Trouver la fitness minimale
            if (adjusted_fitness < min_fitness) {
                min_fitness = adjusted_fitness;
            }
        }
 else {
    std::cerr << "Erreur: Génome introuvable dans fitness_map !" << std::endl;
    continue; // Passer au prochain génome
}

            

        // Appliquer une translation si nécessaire
        if (min_fitness < 0) {
            for (double& fitness : adjusted_fitnesses) {
                fitness += std::abs(min_fitness) + 1.0; // Translation pour rendre toutes les fitness positives
            }
        }

        // Reproduire au sein de l'espèce
        while (new_generation.size() < config.population_size) {
            const auto &p1 = rng.roulette_selection(species.members, adjusted_fitnesses);
            const auto &p2 = rng.roulette_selection(species.members, adjusted_fitnesses);

            // Crossover
            neat::Neat neat_instance;
            Genome offspring_genome = neat_instance.alt_crossover(p1, p2, generate_next_genome_id());
            std::shared_ptr<Genome> offspring = std::make_shared<Genome>(offspring_genome);

            // Mutation
            mutate(*offspring);

            // Ajouter à la nouvelle génération
            new_generation.push_back(neat::Individual(offspring));
        }
    }

    return new_generation;
}
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

void Population::replace_population(std::vector<neat::Individual> new_generation) {
    if (new_generation.empty()) {
        throw std::runtime_error("Erreur : La nouvelle génération est vide. Impossible de remplacer la population.");
    }

    // Remplace les individus actuels par ceux de la nouvelle génération
    individuals = std::move(new_generation);

    // Met à jour le meilleur individu avec la nouvelle population
    update_best();

    std::cout << "Population remplacée. Taille actuelle : " << individuals.size() << std::endl;
}

void Population::clear_species() {
        for (auto &species : species_list) {
            species.clear_members();
        }
    }



    // Créer une nouvelle espèce
void Population::create_new_species(std::shared_ptr<Genome> representative) {
    int new_id = species_list.size() + 1;
    Species new_species(new_id, *representative); 
    species_list.push_back(std::move(new_species));
}

std::vector<Species> &Population::get_species_list()
{return species_list;
}

int Population::species_id_counter = 0;


int Population::generate_next_species_id() {
    return species_id_counter ++;
}
