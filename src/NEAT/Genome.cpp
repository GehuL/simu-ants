#include "Genome.h"
#include "neat.h"
#include <optional>
#include <iostream>
#include <vector>
#include <functional>

// Constructeur par défaut
Genome::Genome() : genome_id(0), num_inputs(0), num_outputs(0) {}

Genome::Genome(int id, int num_inputs, int num_outputs)
    : genome_id(id), num_inputs(num_inputs), num_outputs(num_outputs) {}

// Crée un nouveau génome avec les neurones d'entrée, de sortie et un certain nombre de neurones cachés
// Fonction auxiliaire pour vérifier si un lien créerait un cycle
bool Genome::would_create_cycle(int input_id, int output_id) const {
    std::unordered_set<int> visited;
    std::unordered_map<int, std::vector<int>> graph;

    // Construire le graphe actuel des connexions
    for (const auto &link : links) {
        if (link.is_enabled) {
            graph[link.link_id.input_id].push_back(link.link_id.output_id);
        }
    }

    // Effectuer une recherche en profondeur pour voir s'il existe un chemin de output_id vers input_id
    std::function<bool(int)> dfs = [&](int current) {
        if (current == input_id) return true;
        if (visited.count(current)) return false;

        visited.insert(current);
        for (int neighbor : graph[current]) {
            if (dfs(neighbor)) return true;
        }
        return false;
    };

    return dfs(output_id);
}

// Fonction de création du génome avec vérification des cycles
Genome Genome::create_genome(int id, int num_inputs, int num_outputs, int num_hidden_neurons, RNG &rng) {
    Genome genome(id, num_inputs, num_outputs);

    // Ajoute neurones d'entrée
    for (int i = 0; i < num_inputs; ++i) {
        genome.add_neuron(neat::NeuronGene{i, 0.0, Activation(Activation::Type::Sigmoid)});
    }

    // Ajoute neurones de sortie
    for (int i = 0; i < num_outputs; ++i) {
        int output_id = num_inputs + i;
        genome.add_neuron(neat::NeuronGene{output_id, 0.0, Activation(Activation::Type::Sigmoid)});
    }

    // Ajoute neurones cachés
    for (int i = 0; i < num_hidden_neurons; ++i) {
        int hidden_id = num_inputs + num_outputs + i;
        genome.add_neuron(neat::NeuronGene{hidden_id, 0.0, Activation(Activation::Type::Sigmoid)});
    }

    // Liens : entrée -> cachés
    for (int input_id = 0; input_id < num_inputs; ++input_id) {
        for (int hidden_id = num_inputs + num_outputs; hidden_id < num_inputs + num_outputs + num_hidden_neurons; ++hidden_id) {
            if (!genome.would_create_cycle(input_id, hidden_id)) {
                genome.add_link(genome.create_link(input_id, hidden_id, rng));
            }
        }
    }

    // Liens : cachés -> cachés (pour favoriser l'émergence de structures complexes)
    for (int hidden_id = num_inputs + num_outputs; hidden_id < num_inputs + num_outputs + num_hidden_neurons; ++hidden_id) {
        for (int target_hidden_id = hidden_id + 1; target_hidden_id < num_inputs + num_outputs + num_hidden_neurons; ++target_hidden_id) {
            if (!genome.would_create_cycle(hidden_id, target_hidden_id)) {
                genome.add_link(genome.create_link(hidden_id, target_hidden_id, rng));
            }
        }
    }

    // Liens : cachés -> sorties
    for (int hidden_id = num_inputs + num_outputs; hidden_id < num_inputs + num_outputs + num_hidden_neurons; ++hidden_id) {
        for (int output_id = num_inputs; output_id < num_inputs + num_outputs; ++output_id) {
            if (!genome.would_create_cycle(hidden_id, output_id)) {
                genome.add_link(genome.create_link(hidden_id, output_id, rng));
            }
        }
    }

    // Affichage pour débogage
    /*
    std::cout << "Genome ID: " << id << std::endl;
    std::cout << "Neurones:" << std::endl;
    for (const auto &neuron : genome.get_neurons()) {
        std::cout << "  Neuron ID: " << neuron.neuron_id 
                  << ", Biais: " << neuron.bias << std::endl;
    }
    std::cout << "Liens:" << std::endl;
    for (const auto &link : genome.get_links()) {
        std::cout << "  Link from " << link.link_id.input_id 
                  << " to " << link.link_id.output_id 
                  << " with weight " << link.weight << std::endl;
    }
    */

    return genome;
}

Genome Genome::create_genome_div(int id, int num_inputs, int num_outputs, int num_hidden_neurons, RNG &rng) {
    Genome genome(id, num_inputs, num_outputs);

    // Ajoute neurones d'entrée
    for (int i = 0; i < num_inputs; ++i) {
        // Les neurones d'entrée n'ont pas de biais, mais peuvent en avoir si nécessaire
        genome.add_neuron(neat::NeuronGene{i, 0.0, Activation(Activation::Type::Sigmoid)});
    }

    // Ajoute neurones de sortie avec des biais initiaux aléatoires
    for (int i = 0; i < num_outputs; ++i) {
        int output_id = num_inputs + i;
        double bias = rng.uniform(-1.0, 1.0); // Distribution uniforme entre -1 et 1
        genome.add_neuron(neat::NeuronGene{output_id, bias, Activation(Activation::Type::Sigmoid)});
    }

    // Ajoute neurones cachés avec des biais initiaux aléatoires
    for (int i = 0; i < num_hidden_neurons; ++i) {
        int hidden_id = num_inputs + num_outputs + i;
        double bias = rng.gaussian(0.0, 1.0); // Distribution gaussienne centrée sur 0 avec écart-type 1
        genome.add_neuron(neat::NeuronGene{hidden_id, bias, Activation(Activation::Type::Sigmoid)});
    }

     // Liens : entrée -> cachés
    for (int input_id = 0; input_id < num_inputs; ++input_id) {
        for (int hidden_id = num_inputs + num_outputs; hidden_id < num_inputs + num_outputs + num_hidden_neurons; ++hidden_id) {
            if (!genome.would_create_cycle(input_id, hidden_id)) {
                genome.add_link(genome.create_link_div(input_id, hidden_id, rng));
            }
        }
    }

    // Liens : cachés -> cachés (pour favoriser l'émergence de structures complexes)
    for (int hidden_id = num_inputs + num_outputs; hidden_id < num_inputs + num_outputs + num_hidden_neurons; ++hidden_id) {
        for (int target_hidden_id = hidden_id + 1; target_hidden_id < num_inputs + num_outputs + num_hidden_neurons; ++target_hidden_id) {
            if (!genome.would_create_cycle(hidden_id, target_hidden_id)) {
                genome.add_link(genome.create_link_div(hidden_id, target_hidden_id, rng));
            }
        }
    }

    // Liens : cachés -> sorties
    for (int hidden_id = num_inputs + num_outputs; hidden_id < num_inputs + num_outputs + num_hidden_neurons; ++hidden_id) {
        for (int output_id = num_inputs; output_id < num_inputs + num_outputs; ++output_id) {
            if (!genome.would_create_cycle(hidden_id, output_id)) {
                genome.add_link(genome.create_link_div(hidden_id, output_id, rng));
            }
        }
    }


    return genome;
}

Genome Genome::create_diverse_genome(int id, int num_inputs, int num_outputs, int max_hidden_neurons, RNG &rng) {
    Genome genome(id, num_inputs, num_outputs);

    // Ajoute neurones d'entrée
    for (int i = 0; i < num_inputs; ++i) {
        genome.add_neuron(neat::NeuronGene{i, 0.0, Activation(Activation::Type::Sigmoid)});
    }

    // Ajoute neurones de sortie avec des biais aléatoires
    for (int i = 0; i < num_outputs; ++i) {
        int output_id = num_inputs + i;
        double bias = rng.uniform(-1.0, 1.0);
        genome.add_neuron(neat::NeuronGene{output_id, bias, Activation(Activation::Type::Sigmoid)});
    }

    // Nombre aléatoire de neurones cachés (jusqu'à max_hidden_neurons)
    int num_hidden_neurons = rng.uniform(0, max_hidden_neurons);
    for (int i = 0; i < num_hidden_neurons; ++i) {
        int hidden_id = num_inputs + num_outputs + i;
        double bias = rng.gaussian(0.0, 1.0);
        
        // Fonction d'activation aléatoire
        Activation::Type activation_type = rng.uniform(0, 2) == 0 
            ? Activation::Type::Sigmoid 
            : Activation::Type::ReLU;
        
        genome.add_neuron(neat::NeuronGene{hidden_id, bias, Activation(activation_type)});
    }

    // Ajoute connexions aléatoires (entrée -> caché, caché -> caché, caché -> sortie)
    auto add_random_connection = [&](int from, int to) {
        if (!genome.would_create_cycle(from, to)) {
            genome.add_link(genome.create_link_div(from, to, rng));
        }
    };

    // Probabilités pour les connexions
    double prob_input_to_hidden = 0.5;
    double prob_hidden_to_hidden = 0.3;
    double prob_hidden_to_output = 0.7;

    // Connexions : entrée -> cachés
    for (int input_id = 0; input_id < num_inputs; ++input_id) {
        for (int hidden_id = num_inputs + num_outputs; hidden_id < num_inputs + num_outputs + num_hidden_neurons; ++hidden_id) {
            if (rng.uniform(0.0, 1.0) < prob_input_to_hidden) {
                add_random_connection(input_id, hidden_id);
            }
        }
    }

    // Connexions : cachés -> cachés
    for (int hidden_id = num_inputs + num_outputs; hidden_id < num_inputs + num_outputs + num_hidden_neurons; ++hidden_id) {
        for (int target_hidden_id = hidden_id + 1; target_hidden_id < num_inputs + num_outputs + num_hidden_neurons; ++target_hidden_id) {
            if (rng.uniform(0.0, 1.0) < prob_hidden_to_hidden) {
                add_random_connection(hidden_id, target_hidden_id);
            }
        }
    }

    // Connexions : cachés -> sorties
    for (int hidden_id = num_inputs + num_outputs; hidden_id < num_inputs + num_outputs + num_hidden_neurons; ++hidden_id) {
        for (int output_id = num_inputs; output_id < num_inputs + num_outputs; ++output_id) {
            if (rng.uniform(0.0, 1.0) < prob_hidden_to_output) {
                add_random_connection(hidden_id, output_id);
            }
        }
    }

    return genome;
}

/*
double Genome::compute_distance(const Genome &other, const NeatConfig &config) const {
    int num_disjoint = 0;
    int num_excess = 0;
    double weight_diff = 0.0;
    int matching_genes = 0;

    auto it1 = links.begin();
    auto it2 = other.links.begin();

    while (it1 != links.end() || it2 != other.links.end()) {
        if (it1 == links.end()) {
            ++num_excess;
            ++it2;
        } else if (it2 == other.links.end()) {
            ++num_excess;
            ++it1;
        } else if (it1->innovation_number < it2->innovation_number) {
            ++num_disjoint;
            ++it1;
        } else if (it1->innovation_number > it2->innovation_number) {
            ++num_disjoint;
            ++it2;
        } else {
            // Matching genes
            weight_diff += std::abs(it1->weight - it2->weight);
            ++matching_genes;
            ++it1;
            ++it2;
        }
    }

    int max_genes = std::max(links.size(), other.links.size());
    double avg_weight_diff = (matching_genes > 0) ? (weight_diff / matching_genes) : 0.0;

    return (config.compatibility_coefficient_excess * num_excess) / max_genes +
           (config.compatibility_coefficient_disjoint * num_disjoint) / max_genes +
           (config.compatibility_coefficient_weights * avg_weight_diff);
}


*/




// Crée un lien avec des poids aléatoires
neat::LinkGene Genome::create_link(int input_id, int output_id, RNG &rng) {
    return neat::LinkGene{{input_id, output_id}, rng.next_gaussian(0.0, 1.0), true};
}

neat::LinkGene Genome::create_link_div(int input_id, int output_id, RNG &rng) {
    double weight = (rng.next_bool()) 
        ? rng.uniform(-3.0, 3.0)  // Poids uniformes
        : rng.next_gaussian(0.0, 2.0);  // Poids gaussiens
    return neat::LinkGene{{input_id, output_id}, weight, true};
}

neat::NeuronGene Genome::create_neuron(int neuron_id) {
    return neat::NeuronGene{neuron_id, 0.0, Activation(Activation::Type::Sigmoid)};
}


int Genome::get_num_inputs() const {
    return num_inputs;  // Retourne le nombre d'entrées
}

int Genome::get_num_outputs() const {
    return num_outputs;  // Retourne le nombre de sorties
}

int Genome::get_genome_id() const {
    return genome_id;  // Retourne l'ID du génome
}

std::vector<neat::NeuronGene> Genome::get_neurons() const {
    return neurons;  // Retourne les neurones du génome
}

std::vector<neat::LinkGene> Genome::get_links() const {
    return links;  // Retourne les liens du génome
}

std::vector<neat::NeuronGene>& Genome::get_neurons() {
    return neurons;  // Retourne les neurones du génome
}

std::vector<neat::LinkGene>& Genome::get_links() {
    return links;  // Retourne les liens du génome
}

int Genome::generate_next_neuron_id() {
    int max_id = 0;
    for (const auto& neuron : neurons) {
        if (neuron.neuron_id > max_id) {
            max_id = neuron.neuron_id;
        }
    }
    return max_id + 1;
}

// Ajout des fonctions de gestion de neurones et liens
void Genome::add_neuron(const neat::NeuronGene &neuron) {
    neurons.push_back(neuron);
}

void Genome::add_link(const neat::LinkGene &link) {
    links.push_back(link);
}

// Recherche un neurone dans le génome par ID
std::optional<neat::NeuronGene> Genome::find_neuron(int neuron_id) const {
    for (const auto &neuron : neurons) {
        if (neuron.neuron_id == neuron_id) {
            return neuron;  // Retourne le neurone s'il est trouvé
        }
    }
    return std::nullopt;  // Retourne un optional vide si non trouvé
}

// Recherche un lien dans le génome par ID de lien
std::optional<neat::LinkGene> Genome::find_link(neat::LinkId link_id) const {
    for (const auto &link : links) {
        if (link.link_id.input_id == link_id.input_id && link.link_id.output_id == link_id.output_id) {
            return link;  // Retourne le lien s'il est trouvé
        }
    }
    return std::nullopt;  // Retourne un optional vide si non trouvé
}

// Génère un vecteur contenant les identifiants des nœuds d’entrée
std::vector<int> Genome::make_input_ids() const {
    std::vector<int> input_ids;
    for (int i = 0; i < num_inputs; i++) {
        input_ids.push_back(i);  // Ajoute les IDs des entrées
    }
    return input_ids;
}

// Génère un vecteur contenant les identifiants des nœuds de sortie
std::vector<int> Genome::make_output_ids() const {
    std::vector<int> output_ids;
    for (int i = 0; i < num_outputs; i++) {
        output_ids.push_back(num_inputs + i);  // Ajoute les IDs des sorties
    }
    return output_ids;
}
