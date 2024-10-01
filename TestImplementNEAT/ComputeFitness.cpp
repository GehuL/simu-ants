#include "ComputeFitness.h"
#include "Genome.h"
#include <iostream>

// Constructeur qui initialise la référence RNG
ComputeFitness::ComputeFitness(RNG &rng) : rng(rng) {}

// Surcharge de l'opérateur () pour évaluer la fitness d'un génome
double ComputeFitness::operator()(const Genome &genome) const {
    return evaluate(genome);  // Appelle la méthode d'évaluation interne
}

// Méthode pour évaluer la fitness d'un génome
double ComputeFitness::evaluate(const Genome &genome) const {
    // Ici, vous implémentez la logique de fitness spécifique à votre projet.
    // Cela peut impliquer de simuler le comportement du génome dans un environnement donné.
    
    std::cout << "Evaluating genome ID: " << genome.get_genome_id() << std::endl;

    // Vous pouvez retourner un score de fitness basé sur votre logique :
    // Par exemple, renvoyer une valeur aléatoire pour simuler la fitness
    return rng.next_double();

    
    }



