// neat.h
#ifndef NEAT_H
#define NEAT_H

#include <vector>
#include "Activation.h"
#include"Genome.h"  

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

class LinkMutator {
public:
    LinkMutator();  // Constructeur

    LinkGene new_value(int input_id, int output_id); // Méthode pour générer un nouveau lien

private:
    double random_weight(); // Fonction privée pour générer un poids aléatoire
};


bool would_create_cycle(const std::vector<neat::LinkGene>& links, int input_id, int output_id);

int choose_random_input_or_hidden_neuron(const std::vector<NeuronGene>& neurons);

int choose_random_output_or_hidden_neuron(const std::vector<NeuronGene>& neurons);


void mutate_add_link(Genome &genome);

void mutate_remove_link(Genome &genome);

} // namespace neat

#endif // NEAT_H
