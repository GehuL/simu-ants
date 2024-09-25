#include "Genome.h"

Genome::Genome(int id, int num_inputs, int num_outputs)
    : genome_id(id), num_inputs(num_inputs), num_outputs(num_outputs) {}

int Genome::get_num_inputs() const {
    return num_inputs;  // Retourne le nombre d'entrées
}

int Genome::get_num_outputs() const {
    return num_outputs;  // Retourne le nombre de sorties
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

