#include "Utils.h"
#include "Population.h"
#include "ComputeFitness.h"
#include "RNG.h"

// Exemple de main
int main(int argc, char* argv[]) {
    RNG rng;
    ComputeFitness compute_fitness(rng);

    // Initialiser la population
    NeatConfig neat_config;  // Assumez que neat_config soit initialisé
    Population population{neat_config, rng};

    // Exécuter la simulation sur un nombre de générations
    int num_generations = 100;
    auto winner = population.run(compute_fitness, num_generations);

    // Sauvegarder le génome gagnant
    std::string winner_filename = "winner_genome.txt";
    save(winner.genome, winner_filename);

    return 0;
}
