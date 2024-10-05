#include <iostream>
#include <vector>
#include "Genome.h"           // Inclure la définition de Genome
#include "NeuralNetwork.h"    // Inclure la définition de FeedForwardNeuralNetwork
#include "Utils.h"            // Inclure la définition de save
#include "neat.h"             // Inclure neat.h pour les définitions complètes

int main() {
    // Création d'un exemple de génome
    Genome genome(1, 2, 1); // ID: 1, 2 entrées, 1 sortie

    // Ajout d'exemples de neurones
    genome.add_neuron(neat::NeuronGene{0, 0.5, Activation{Activation::Type::Sigmoid}});
    genome.add_neuron(neat::NeuronGene{1, -0.3, Activation{Activation::Type::Tanh}});
    genome.add_neuron(neat::NeuronGene{2, 0.1, Activation{Activation::Type::Sigmoid}});

    // Ajout d'exemples de liens
    genome.add_link(neat::LinkGene{neat::LinkId{0, 1}, 0.8, true});

    // Sauvegarde du génome dans un fichier
    std::string filename = "genome.txt";
    save(genome, filename);

    // Création d'un réseau de neurones à partir du génome
    FeedForwardNeuralNetwork nn = create_from_genome(genome);

    // Test d'activation du réseau avec des entrées
    std::vector<double> inputs = {1.0, 0.5}; // Exemple d'entrée
    std::vector<double> outputs = nn.activate(inputs);

    // Affichage des sorties
    std::cout << "Outputs: ";
    for (double output : outputs) {
        std::cout << output << " ";
    }
    std::cout << std::endl;

    return 0;
}
