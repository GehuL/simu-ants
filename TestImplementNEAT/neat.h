// neat.h
#ifndef NEAT_H
#define NEAT_H

#include <vector>
#include "Activation.h"

namespace neat {

// Structure d'un neurone dans le génome
struct NeuronGene {
    int neuron_id;
    double bias;
    Activation activation;  // Enum ou fonction d'activation
};

// Identifiants de la connexion entre deux neurones
struct LinkId {
    int input_id;
    int output_id;
};

// Structure représentant une connexion entre deux neurones
struct LinkGene {
    LinkId link_id;    // Connexion entre les neurones
    double weight;     // Poids de la connexion
    bool is_enabled;   // Si la connexion est active
};



struct Individual {
    Genome genome;
    double fitness;
};

// Classe Neat contenant les méthodes d'évolution
class Neat {
public:
    NeuronGene crossover_neuron(const NeuronGene &a, const NeuronGene &b);
    

    LinkGene crossover_link(const LinkGene &a, const LinkGene &b) ;

    Genome crossover(const Individual &dominant, const Individual &recessive);
private:
    GenomeIndexer m_genome_indexer;

};


} // namespace neat

#endif // NEAT_H
