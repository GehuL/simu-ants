#include "Genome.h"
#include "neat.h"
#include <optional>
#include <vector>

// Constructeur par défaut
Genome::Genome() : genome_id(0), num_inputs(0), num_outputs(0) {}

Genome::Genome(int id, int num_inputs, int num_outputs)
    : genome_id(id), num_inputs(num_inputs), num_outputs(num_outputs) {}

// Crée un nouveau génome avec les neurones d'entrée, de sortie et un certain nombre de neurones cachés
Genome Genome::create_genome(int id, int num_inputs, int num_outputs, int num_hidden_neurons, RNG &rng) {
    Genome genome(id, num_inputs, num_outputs);

    // Ajoute neurones d'entrée
    for (int i = 0; i < num_inputs; ++i) {
        genome.add_neuron(neat::NeuronGene{i, 0.0, Activation(Activation::Type::Sigmoid)});
    }

    // Ajoute neurones de sortie
    for (int i = 0; i < num_outputs; ++i) {
        genome.add_neuron(neat::NeuronGene{num_inputs + i, 0.0, Activation(Activation::Type::Sigmoid)});
    }

    // Ajoute neurones cachés
    for (int i = 0; i < num_hidden_neurons; ++i) {
        int hidden_id = num_inputs + num_outputs + i;
        genome.add_neuron(neat::NeuronGene{hidden_id, 0.0, Activation(Activation::Type::Sigmoid)});
    }

    return genome;
}

// Crée un lien avec des poids aléatoires
neat::LinkGene Genome::create_link(int input_id, int output_id, RNG &rng) {
    return neat::LinkGene{{input_id, output_id}, rng.next_gaussian(0.0, 1.0), true};
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
