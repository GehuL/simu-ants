#ifndef NEATCONFIG_H
#define NEATCONFIG_H

struct NeatConfig {
    int population_size = 100;        // Taille de la population
    int num_inputs = 12;               // Nombre d'entrées
    int num_outputs = 4;              // Nombre de sorties

    double probability_add_link = 0.2;       // Favorise des connexions progressives
    double probability_remove_link = 0.1;   // Réduit légèrement la complexité
    double probability_add_neuron = 0.1;    // Ajout progressif de neurones
    double probability_remove_neuron = 0.05; // Supprime les neurones inutiles
    double probability_mutate_link_weight = 0.7;  // Explore efficacement l'espace des solutions
    double probability_mutate_neuron_bias = 0.6;  // Ajuste le comportement des neurones
    double survival_threshold = 0.3;       // Maintient un équilibre entre diversité et performance

};

#endif // NEATCONFIG_H
