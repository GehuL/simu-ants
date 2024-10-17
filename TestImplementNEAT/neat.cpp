// neat.cpp
#include "neat.h"
#include "rng.h"  // Inclusion de la classe RNG
#include "GenomeIndexer.h"
#include"Genome.h"  
#include <cassert>
#include <algorithm> // For std::clamp
#include <optional>
#include <unordered_set>
#include <functional>
#include "NeuronMutator.h"
#include <iostream>

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

Genome Neat::crossover(const Individual &dominant, const Individual &recessive, int child_genome_id) {
    Genome offspring{child_genome_id, dominant.genome.get_num_inputs(), dominant.genome.get_num_outputs()};

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

    return offspring;
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

int choose_random_input_or_hidden_neuron(const std::vector<NeuronGene>& neurons) {
    std::vector<int> valid_neurons;
    NeatConfig config;

    for (const auto& neuron : neurons) {
        // Neurones d'entrée : ID entre 0 et num_inputs - 1
        // Neurones cachés : ID > num_inputs + num_outputs - 1
        if (neuron.neuron_id < config.num_inputs || 
            neuron.neuron_id >= config.num_inputs + config.num_outputs) {
            valid_neurons.push_back(neuron.neuron_id);
        }
    }

    if (valid_neurons.empty()) {
        return -1; // ou gérer l'erreur autrement
    }

    int random_index = std::rand() % valid_neurons.size();
    return valid_neurons[random_index];
}


int choose_random_output_or_hidden_neuron(const std::vector<NeuronGene>& neurons) {
    std::vector<int> valid_neurons;
    NeatConfig config;

    for (const auto& neuron : neurons) {
        // Neurones de sortie : ID entre num_inputs et num_inputs + num_outputs - 1
        if (neuron.neuron_id >= config.num_inputs && 
            neuron.neuron_id < config.num_inputs + config.num_outputs) {
            valid_neurons.push_back(neuron.neuron_id);
        }
    }
    if (valid_neurons.empty()) {
        return -1; // ou gérer l'erreur autrement
    }
    int random_index = std::rand() % valid_neurons.size();
    return valid_neurons[random_index];
}

// Fonction pour choisir un neurone caché aléatoire
std::vector<NeuronGene>::const_iterator choose_random_hidden(std::vector<NeuronGene>& neurons) {
    std::vector<std::vector<NeuronGene>::const_iterator> hidden_neurons;
    NeatConfig config;
    
    // Supposons que vous avez une façon de déterminer si un neurone est caché (ID > num_inputs + num_outputs - 1)
    for (auto it = neurons.begin(); it != neurons.end(); ++it) {
        if (it->neuron_id >= config.num_inputs + config.num_outputs) {
            hidden_neurons.push_back(it);
        }
    }


    if (hidden_neurons.empty()) {
        throw std::out_of_range("No hidden neurons available.");
    }

    RNG rng; // Assurez-vous que RNG est bien défini
    return rng.choose_random(hidden_neurons); // Utilisez votre méthode choose_random
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

    if (input_id == -1 || output_id == -1) {
        return; // Gérer l'erreur si aucun neurone valide n'est trouvé
    }

    LinkId link_id{input_id, output_id};

    // Vérifier si le lien existe déjà
    auto existing_link = genome.find_link(link_id);
    if (existing_link) {
        // Réactiver le lien existant s'il n'est pas déjà activé
        if (!existing_link->is_enabled) {
            existing_link->is_enabled = true;
        }
        return;
    }

    // Ne supporte que les réseaux feedforward
    // Vérifier si l'ajout du lien créerait un cycle
    if (would_create_cycle(genome.links, input_id, output_id)) {
        return; // Ne pas ajouter le lien si cela crée un cycle
    }

    // Créer le nouveau lien
    LinkMutator link_mutator; // Instancier link_mutator
    neat::LinkGene new_link = link_mutator.new_value(input_id, output_id);
    genome.add_link(new_link);
}


void dfs(int neuron_id, const std::unordered_map<int, std::vector<int>>& graph, std::unordered_set<int>& visited) {
    // Vérifiez si neuron_id existe dans le graphe
    auto it = graph.find(neuron_id);
    if (it == graph.end()) {
        std::cerr << "Erreur : Neuron ID " << neuron_id << " introuvable dans le graphe." << std::endl;
        return;  // Sortir de la fonction si le neurone n'est pas trouvé
    }

    // Continuez avec l'accès aux voisins
    visited.insert(neuron_id);
    for (int neighbor : it->second) {
        if (visited.find(neighbor) == visited.end()) {
            dfs(neighbor, graph, visited);
        }
    }
}


void mutate_remove_link(Genome &genome) {
    RNG rng;
    NeatConfig config;

    // Si le génome n'a pas de liens, rien à faire
    if (genome.links.empty()) {
        return;
    }

    // Identifier les liens critiques
    std::unordered_set<neat::LinkId, neat::LinkIdHash> essential_links;

    // Ajouter les liens d'entrée aux neurones cachés
    for (const auto& neuron : genome.neurons) {
        if (neuron.neuron_id < config.num_inputs) {  // Neurones d'entrée
            for (const auto& link : genome.links) {
                if (link.link_id.input_id == neuron.neuron_id) {
                    essential_links.insert(link.link_id);
                }
            }
        }
    }

    // Ajouter les liens des neurones cachés aux neurones de sortie
    for (const auto& neuron : genome.neurons) {
        if (neuron.neuron_id >= config.num_inputs && 
            neuron.neuron_id < config.num_inputs + config.num_outputs) { // Neurones de sortie
            for (const auto& link : genome.links) {
                if (link.link_id.output_id == neuron.neuron_id) {
                    essential_links.insert(link.link_id);
                }
            }
        }
    }

    // Ajouter les liens entre neurones cachés
    for (const auto& link : genome.links) {
        if (link.link_id.input_id >= config.num_inputs && link.link_id.output_id >= config.num_inputs) {
            essential_links.insert(link.link_id);
        }
    }

    // Filtrer les liens non essentiels
    std::vector<LinkGene> removable_links;
    for (const auto& link : genome.links) {
        if (essential_links.find(link.link_id) == essential_links.end()) {
            removable_links.push_back(link);
        }
    }

    // Si aucun lien n'est remplaçable, on sort
    if (removable_links.empty()) {
        return;
    }

    // Choisir un lien à supprimer parmi ceux qui ne sont pas essentiels
    auto to_remove = rng.choose_random(removable_links);
    genome.links.erase(std::remove(genome.links.begin(), genome.links.end(), to_remove), genome.links.end());
}

/*
void mutate_remove_link(Genome &genome) {
    RNG rng;
    NeatConfig config;

    // Si le génome n'a pas de liens, rien à faire
    if (genome.links.empty()) {
        return;
    }

    // Construire le graphe des neurones
    std::unordered_map<int, std::vector<int>> graph;
    for (const auto& link : genome.links) {
        graph[link.link_id.input_id].push_back(link.link_id.output_id);
    }

    // Effectuer un DFS pour identifier les neurones atteignables
    std::unordered_set<int> visited;
    for (int i = 0; i < config.num_inputs; ++i) {
        if (graph.find(i) != graph.end()) {
            dfs(i, graph, visited);
        }
    }

    // Identifier les liens essentiels basés sur les neurones visités
    std::unordered_set<neat::LinkId, neat::LinkIdHash> essential_links;
    for (const auto& link : genome.links) {
        if (visited.find(link.link_id.input_id) != visited.end() &&
            visited.find(link.link_id.output_id) != visited.end()) {
            essential_links.insert(link.link_id);
        }
    }

    // Filtrer les liens non essentiels
    std::vector<LinkGene> removable_links;
    for (const auto& link : genome.links) {
        if (essential_links.find(link.link_id) == essential_links.end()) {
            removable_links.push_back(link);
        }
    }

    // Choisir un lien à supprimer parmi ceux qui ne sont pas essentiels
    if (!removable_links.empty()) {
        auto to_remove = rng.choose_random(removable_links);
        genome.links.erase(std::remove(genome.links.begin(), genome.links.end(), to_remove), genome.links.end());
    }
}

*/


void mutate_add_neuron(Genome &genome)
{
    RNG rng;

    if (genome.links.empty()) {
        return;
    }

    neat::LinkGene link_to_split = rng.choose_random(genome.links);  // Choisir un lien à diviser

    // Désactiver le lien existant
    link_to_split.is_enabled = false;

    // Retirer le lien divisé en fonction de son LinkId
    genome.links.erase(std::remove_if(genome.links.begin(), genome.links.end(),
        [&link_to_split](const neat::LinkGene &link) {
            return link.link_id == link_to_split.link_id;
        }), genome.links.end());

    NeuronMutator neuron_mutator;  // Instancier NeuronMutator
    neat::NeuronGene new_neuron = neuron_mutator.new_neuron();  // Créer un nouveau neurone
    new_neuron.neuron_id = genome.generate_next_neuron_id();  // Générer un nouvel ID de neurone
    genome.add_neuron(new_neuron);  // Ajouter le nouveau neurone

    neat::LinkId link_id = link_to_split.link_id;
    double weight = link_to_split.weight;

    // Ajouter un lien du neurone d'entrée au nouveau neurone
    genome.add_link(neat::LinkGene{{link_id.input_id, new_neuron.neuron_id}, 1.0, true});  
    // Ajouter un lien du nouveau neurone au neurone de sortie
    genome.add_link(neat::LinkGene{{new_neuron.neuron_id, link_id.output_id}, weight, true});  
}



void mutate_remove_neuron(Genome &genome) {
    // Vérifier qu'il reste au moins 2 neurones cachés
    int hidden_neuron_count = std::count_if(genome.neurons.begin(), genome.neurons.end(), 
        [](const NeuronGene &neuron) { 
            NeatConfig config;
            return neuron.is_hidden(neuron.neuron_id,config);  // Assurez-vous que la méthode is_hidden() est définie pour détecter les neurones cachés
        });

    if (hidden_neuron_count < 2) {
        return;  // Ne rien faire s'il reste moins de 2 neurones cachés
    }

    // Choisir un neurone caché aléatoire
    RNG rng;
    auto neuron_it = choose_random_hidden(genome.neurons);

    // Effacer les liens qui pointent vers ce neurone
    genome.links.erase(std::remove_if(genome.links.begin(), genome.links.end(), 
        [neuron_it](const LinkGene &link) {
            return link.has_neuron(*neuron_it);  // Utilisez le neurone déférencé
        }),
        genome.links.end());

    // Supprimer le neurone
    genome.neurons.erase(neuron_it);  // Utilisez l'itérateur ici
}


double clamp(double x){
    DoubleConfig config;
    return std::min(config.max_value, std::max(config.min_value, x));
}


double new_value(){
    RNG rng;
    DoubleConfig config;
    return clamp(rng.next_gaussian(config.init_mean, config.init_stdev));
}

double mutate_delta(double value){
    RNG rng;
    DoubleConfig config;
    double delta = clamp( rng.next_gaussian(0, config.mutate_power));
    return clamp (value + delta);
}





}  // namespace neat


