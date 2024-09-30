#include "Utils.h"
#include <fstream>
#include <iostream>
#include "neat.h"

// Implémentation de la fonction save
void save(const Genome &genome, const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    // Sauvegarder les informations du génome dans le fichier
    file << "Genome ID: " << genome.get_genome_id() << "\n";
    file << "Neurons:\n";
    for (const auto &neuron : genome.neurons) {
        file << "Neuron ID: " << neuron.neuron_id << ", Bias: " << neuron.bias << "\n";
    }

    file << "Links:\n";
    for (const auto &link : genome.links) {
        file << "Link from neuron " << link.link_id.input_id 
             << " to neuron " << link.link_id.output_id 
             << " with weight " << link.weight 
             << (link.is_enabled ? " (enabled)" : " (disabled)") << "\n";
    }

    file.close();
    std::cout << "Genome saved to " << filename << std::endl;
}
