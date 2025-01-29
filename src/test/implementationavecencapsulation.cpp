#include <iostream>
#include <vector>
#include "Ant.h"
#include "ComputeFitness.h"
#include "Population.h"

int main() {
    // Initialisation de la configuration NEAT et des objets associés
    NeatConfig config;
    RNG rng;
    ComputeFitness compute_fitness(rng);
    Population population(config, rng);

    // Crée les fourmis avec les génomes de la population initiale
    std::vector<Ant> ants;
    for (const auto& individual : population.get_individuals()) {
        ants.emplace_back(individual.genome);
    }

    // Simulation des générations
    for (int generation = 0; generation < max_generations; ++generation) {
        bool all_reached_goal = false;

        // Boucle de simulation jusqu'à ce que toutes les fourmis atteignent leur objectif
        while (!all_reached_goal) {
            all_reached_goal = true;

            for (size_t i = 0; i < ants.size(); ++i) {
                // Récupérer l'état du jeu pour cette fourmi
                std::vector<double> game_state = get_game_state(ants[i]);

                // Met à jour l'état de la fourmi
                ants[i].update_state(game_state);

                // Vérifier si la fourmi a atteint la fin du labyrinthe
                if (!ants[i].has_reached_goal()) {
                    all_reached_goal = false; // Une fourmi n'a pas atteint l'arrivée
                }
            }
        }

        // Calcul de la fitness pour chaque fourmi
        for (size_t i = 0; i < ants.size(); ++i) {
            ants[i].update_fitness(compute_fitness);  // Calcule et met à jour la fitness

            // Met à jour la fitness de l'individu correspondant dans la population
            population.get_individual(i).fitness = ants[i].get_fitness();
        }

        // Génère une nouvelle génération d'individus
        auto new_generation = population.reproduce();
        population.set_individuals(new_generation);  // Remplace la population par la nouvelle génération

        // Crée les fourmis pour la nouvelle génération avec les génomes mis à jour
        ants.clear();
        for (const auto& individual : new_generation) {
            ants.emplace_back(individual.genome);
        }
    }

    return 0;
}


// Ant.cpp
#include "Ant.h"
#include <cmath>

Ant::Ant(const Genome& genome) 
    : nn(create_from_genome(genome)), fitness(0), x(0), y(0) {
    // Initialisation du réseau neuronal et de la position
}

void Ant::update_state(const std::vector<double>& game_state) {
    // Utilise le réseau neuronal pour générer des actions en fonction de l'état du jeu
    std::vector<double> actions = nn.activate(game_state);
    perform_action(actions);  // Applique les actions au jeu
}

void Ant::perform_action(const std::vector<double>& actions) {
    
}

bool Ant::has_reached_goal() const {
    
}

double Ant::get_fitness() const {
    
