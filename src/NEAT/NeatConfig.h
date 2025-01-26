#ifndef NEATCONFIG_H
#define NEATCONFIG_H

struct NeatConfig {
    int population_size = 200;        // Taille de la population
    int num_inputs = 19;               // Nombre d'entrées
    int num_outputs = 4;              // Nombre de sorties


    double probability_structure_mutation = 0.4; // Probabilité d'une mutation structurelle
    double probability_weight_or_bias_mutation = 1.00; // Probabilité d'une mutation de poids ou de biais

    double probability_add_link = 0.1;       // Favorise des connexions progressives
    double probability_remove_link = 0.00;   // Réduit légèrement la complexité (Combinaison avec add-neuron fait planter)
    double probability_add_neuron = 0.1;    // Ajout progressif de neurones
    double probability_remove_neuron = 0.00; // Supprime les neurones inutiles

    double probability_mutate_link_weight = 0.8;  // Explore efficacement l'espace des solutions
    double probability_mutate_neuron_bias = 0.6;  // Ajuste le comportement des neurones

    double survival_threshold = 0.3;       // Maintient un équilibre entre diversité et performance

     // Coefficients pour la distance de compatibilité
    double compatibility_coefficient_excess = 1.0;   // c1
    double compatibility_coefficient_disjoint = 1.0; // c2
    double compatibility_coefficient_weights = 0.4; // c3

    // Seuil de compatibilité pour regrouper les espèces
    double compatibility_threshold = 3.0;

};

#endif // NEATCONFIG_H
