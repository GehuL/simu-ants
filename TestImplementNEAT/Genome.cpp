#include "Genome.h"
#include "neat.h"  // Inclure neat.h pour les définitions complètes
#include <optional>  // Inclure <optional> pour utiliser std::optional


Genome::Genome() : genome_id(0), num_inputs(0), num_outputs(0) {}

Genome::Genome(int id, int num_inputs, int num_outputs)
    : genome_id(id), num_inputs(num_inputs), num_outputs(num_outputs) {}

int Genome::get_num_inputs() const {
    return num_inputs;  // Retourne le nombre d'entrées
}

int Genome::get_num_outputs() const {
    return num_outputs;  // Retourne le nombre de sorties
}

int Genome::get_genome_id() const {
    return genome_id;  // Retourne l'ID du génome
}

void Genome::add_neuron(const neat::NeuronGene &neuron) {
    neurons.push_back(neuron);
}

void Genome::add_link(const neat::LinkGene &link) {
    links.push_back(link);
}

std::optional<neat::NeuronGene> Genome::find_neuron(int neuron_id) const {
    for (const auto &neuron : neurons) {
        if (neuron.neuron_id == neuron_id) {
            return neuron;  // Retourne le neurone s'il est trouvé
        }
    }
    return std::nullopt;  // Retourne un optional vide si non trouvé
}

std::optional<neat::LinkGene> Genome::find_link(neat::LinkId link_id) const {
    for (const auto &link : links) {
        if (link.link_id.input_id == link_id.input_id && link.link_id.output_id == link_id.output_id) {
            return link;  // Retourne le lien s'il est trouvé
        }
    }
    return std::nullopt;  // Retourne un optional vide si non trouvé
}

std::vector<int> Genome::make_input_ids() const {
    std::vector<int> input_ids;
    for (int i = 0; i < num_inputs; i++) {
        input_ids.push_back(i);  // Ajoute les IDs des entrées
    }
    return input_ids;
}

std::vector<int> Genome::make_output_ids() const {
    std::vector<int> output_ids;
    for (int i = 0; i < num_outputs; i++) {
        output_ids.push_back(num_inputs + i);  // Ajoute les IDs des sorties
    }
    return output_ids;
}

