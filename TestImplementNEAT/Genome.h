#ifndef GENOME_H
#define GENOME_H

#include <vector>
#include <optional>
#include "neat.h"

class Genome {
public:
    Genome(int id, int num_inputs, int num_outputs);
    std::vector<neat::NeuronGene> neurons;  // Vecteur de neurones
    std::vector<neat::LinkGene> links;      // Vecteur de liens

    int get_num_inputs() const;  // Méthode pour obtenir le nombre d'entrées
    int get_num_outputs() const;  // Méthode pour obtenir le nombre de sorties

    void add_neuron(const neat::NeuronGene &neuron);  // Méthode pour ajouter un neurone
    void add_link(const neat::LinkGene &link);  // Méthode pour ajouter un lien
    std::optional<neat::NeuronGene> find_neuron(int neuron_id) const;  // Méthode pour trouver un neurone
    std::optional<neat::LinkGene> find_link(neat::LinkId link_id) const;  // Méthode pour trouver un lien

    // Autres méthodes nécessaires...

private:
    int genome_id;  // ID du génome
    int num_inputs; // Nombre d'entrées
    int num_outputs; // Nombre de sorties
    
};

#endif // GENOME_H
