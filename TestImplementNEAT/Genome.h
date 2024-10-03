#ifndef GENOME_H
#define GENOME_H

#include <vector>
#include <optional>

// Déclarations anticipées des structures dans le namespace neat
namespace neat {
    struct NeuronGene;
    struct LinkGene;
    struct LinkId;
}

class Genome {
public:
    Genome(int id, int num_inputs, int num_outputs);

    // Vecteurs de neurones et de liens
    std::vector<neat::NeuronGene> neurons;
    std::vector<neat::LinkGene> links;

    // Méthodes pour accéder aux informations du génome
    int get_num_inputs() const;
    int get_num_outputs() const;
    int get_genome_id() const;

    std::vector<int> make_input_ids() const;
    std::vector<int> make_output_ids() const;

    // Méthodes pour manipuler le génome
    void add_neuron(const neat::NeuronGene &neuron);
    void add_link(const neat::LinkGene &link);
    std::optional<neat::NeuronGene> find_neuron(int neuron_id) const;
    std::optional<neat::LinkGene> find_link(neat::LinkId link_id) const;

private:
    int genome_id;  // ID du génome
    int num_inputs; // Nombre d'entrées
    int num_outputs; // Nombre de sorties
};

#endif // GENOME_H
