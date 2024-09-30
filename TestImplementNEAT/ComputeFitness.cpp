#include "ComputeFitness.h"
#include <iostream>

// Constructeur qui initialise la référence RNG
ComputeFitness::ComputeFitness(RNG &rng) : rng(rng) {}

// Méthode pour évaluer la fitness d'un génome
double ComputeFitness::evaluate(const Genome &genome) {
    // Ici, vous implémentez la logique de fitness spécifique à votre projet.
    // Cela peut impliquer de simuler le comportement du génome dans un environnement donné.
    
    // Par exemple, vous pouvez utiliser un modèle aléatoire pour l'instant :
    std::cout << "Evaluating genome ID: " << genome.genome_id << std::endl;

    // Vous pouvez retourner un score de fitness basé sur votre logique :
    // Par exemple, renvoyer une valeur aléatoire pour simuler la fitness
    return rng.next_double();
}
