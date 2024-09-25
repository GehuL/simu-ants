// neat.cpp
#include "neat.h"
#include "rng.h"  // Inclusion de la classe RNG
#include "GenomeIndexer.h"
#include"Genome.h"  
#include <cassert>
#include <optional>
#include <unordered_set>
#include <functional>

namespace neat {

NeuronGene Neat::crossover_neuron(const NeuronGene &a, const NeuronGene &b) {
    assert(a.neuron_id == b.neuron_id);

    RNG rng;  

    int neuron_id = a.neuron_id;
    double bias = rng.choose(0.5, a.bias, b.bias);  // Choix aléatoire du biais
    Activation activation = rng.choose(0.5, a.activation, b.activation);  // Choix aléatoire de l'activation

    return NeuronGene{neuron_id, bias, activation};
}

LinkGene Neat::crossover_link(const LinkGene &a, const LinkGene &b) {
    assert(a.link_id.input_id == b.link_id.input_id);
    assert(a.link_id.output_id == b.link_id.output_id);

    RNG rng;

    LinkId link_id = a.link_id;
    double weight = rng.choose(0.5, a.weight, b.weight);  // Choix aléatoire du poids
    bool is_enabled = rng.choose(0.5, a.is_enabled, b.is_enabled);  // Choix aléatoire de l'activation

    return LinkGene{link_id, weight, is_enabled};
}

Genome Neat::crossover(const Individual &dominant, const Individual &recessive) {
    Genome offspring{m_genome_indexer.next(), dominant.genome.get_num_inputs(), dominant.genome.get_num_outputs()};

    for (const auto &dominant_neuron : dominant.genome.neurons) {
        int neuron_id = dominant_neuron.neuron_id;
        std::optional<neat::NeuronGene> recessive_neuron = recessive.genome.find_neuron(neuron_id);
        if (!recessive_neuron) {
            offspring.add_neuron(dominant_neuron);
        } else {
            offspring.add_neuron(crossover_neuron(dominant_neuron, *recessive_neuron));
        }
    }

    for (const auto &dominant_link : dominant.genome.links) {
        LinkId link_id = dominant_link.link_id;
        std::optional<neat::LinkGene> recessive_link = recessive.genome.find_link(link_id);
        if (!recessive_link) {
            offspring.add_link(dominant_link);
        } else {
            offspring.add_link(crossover_link(dominant_link, *recessive_link));
        }
    }

    return offspring; // N'oublie pas de retourner le génome fils !
}

// Constructeur de LinkMutator
LinkMutator::LinkMutator() {}

// Fonction pour générer un nouveau lien
LinkGene LinkMutator::new_value(int input_id, int output_id) {
    double weight = random_weight(); // Appeler la fonction pour obtenir un poids aléatoire
    bool is_enabled = true; // Le lien est activé par défaut

    LinkId link_id{input_id, output_id}; // Créer un identifiant de lien
    return LinkGene{link_id, weight, is_enabled}; // Retourner le nouveau lien
}

// Fonction privée pour générer un poids aléatoire entre -1.0 et 1.0
double LinkMutator::random_weight() {
    return ((double)std::rand() / RAND_MAX) * 2.0 - 1.0; 
}

int neat::choose_random_input_or_hidden_neuron(const std::vector<NeuronGene>& neurons) {
    std::vector<int> valid_neurons;
    for (const auto& neuron : neurons) {
        // Condition pour vérifier si le neurone est d'entrée ou caché
        if (/* condition ici */) {
            valid_neurons.push_back(neuron.neuron_id);
        }
    }
    if (valid_neurons.empty()) {
        return -1; // ou gérer l'erreur autrement
    }
    int random_index = std::rand() % valid_neurons.size();
    return valid_neurons[random_index];
}

int neat::choose_random_output_or_hidden_neuron(const std::vector<NeuronGene>& neurons) {
    std::vector<int> valid_neurons;
    for (const auto& neuron : neurons) {
        // Condition pour vérifier si le neurone est de sortie ou caché
        if (/* condition ici */) {
            valid_neurons.push_back(neuron.neuron_id);
        }
    }
    if (valid_neurons.empty()) {
        return -1; // ou gérer l'erreur autrement
    }
    int random_index = std::rand() % valid_neurons.size();
    return valid_neurons[random_index];
}

bool would_create_cycle(const std::vector<neat::LinkGene>& links, int input_id, int output_id) {
    std::unordered_set<int> visited;  // Pour suivre les neurones déjà visités

    // Fonction récursive pour effectuer la recherche
    std::function<bool(int)> dfs = [&](int current_id) {
        // Si nous avons déjà visité ce neurone, nous avons trouvé un cycle
        if (visited.find(current_id) != visited.end()) {
            return true;
        }

        visited.insert(current_id);  // Marquer le neurone comme visité

        // Explorer les liens sortants
        for (const auto& link : links) {
            if (link.link_id.input_id == current_id) {
                // Si nous atteignons le neurone d'entrée, un cycle est créé
                if (link.link_id.output_id == input_id) {
                    return true;
                }
                // Continuer à explorer le neurone de sortie
                if (dfs(link.link_id.output_id)) {
                    return true;
                }
            }
        }
        return false;  // Aucun cycle trouvé
    };

    // Démarrer la recherche à partir du neurone de sortie
    return dfs(output_id);
}



void mutate_add_link(Genome &genome)
{
    int input_id = choose_random_input_or_hidden_neuron(genome.neurons);
    int output_id = choose_random_output_or_hidden_neuron(genome.neurons);
    LinkId link_id{input_id, output_id};

    auto existing_link = genome.find_link(link_id);
    if (existing_link) {
        existing_link->is_enabled = true;
        return;
}
    //Only support feedforward networks
    if (would_create_cycle(genome.links,input_id,output_id)) {
        return;
        
    }
    LinkMutator link_mutator; // Instancier link_mutator
    neat::LinkGene new_link = link_mutator.new_value(input_id, output_id);
    genome.add_link(new_link);
}

void mutate_remove_link(Genome &genome)
{
    RNG rng;

    if (genome.links.empty()) {
        return;
    }
    auto to_remove = rng.choose_random(genome.links);  // Choisir un lien à supprimer
    genome.links.erase(std::remove(genome.links.begin(), genome.links.end(), to_remove), genome.links.end());
}

} // namespace neat
