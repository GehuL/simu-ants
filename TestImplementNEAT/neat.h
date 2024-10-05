// neat.h
#ifndef NEAT_H
#define NEAT_H

#include <vector>
#include "Activation.h"
#include "Genome.h" 
#include "GenomeIndexer.h"
#include "NeatConfig.h"

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

     // Définir l'opérateur == pour comparer deux LinkId
        bool operator==(const LinkId& other) const {
            return input_id == other.input_id && output_id == other.output_id;
        }
};

// Structure représentant une connexion entre deux neurones
struct LinkGene {
    LinkId link_id;    // Connexion entre les neurones
    double weight;     // Poids de la connexion
    bool is_enabled;   // Si la connexion est active

    // Définir l'opérateur == pour comparer deux LinkGene
        bool operator==(const LinkGene& other) const {
            return link_id == other.link_id && weight == other.weight && is_enabled == other.is_enabled;
        }

    bool has_neuron(const NeuronGene& neuron) const {
        return link_id.input_id == neuron.neuron_id || link_id.output_id == neuron.neuron_id;
    }
};



// Structure pour représenter un individu
struct Individual {
    Genome genome;
    bool fitness_computed;
    double fitness;

    Individual() 
        : genome(), fitness_computed(false), fitness(0.0) {}

    Individual(const Genome &genome)
        : genome(genome), fitness_computed(false), fitness(0.0) {}
};

struct DoubleConfig {
    double init_mean = 0.0;
    double init_stdev = 1.0;
    double min_value = -20.0;
    double max_value = 20.0;
    double mutation_rate = 0.2;
    double mutate_power = 1.2;
    double replace_rate = 0.05;
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

std::vector<NeuronGene>::const_iterator choose_random_hidden(std::vector<NeuronGene>& neurons);



void mutate_add_link(Genome &genome);

void mutate_remove_link(Genome &genome);

void mutate_add_neuron(Genome &genome);

void mutate_remove_neuron(Genome &genome);

double clamp(double x);
double new_value();
double mutate_delta(double value);

} // namespace neat

#endif // NEAT_H
