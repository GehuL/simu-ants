#ifndef GENOME_H
#define GENOME_H

#include "Neat.h"
#include "Activation.h"
#include "rng.h"
#include <vector>
#include <optional>
#include <iostream>
#include "../external/json.hpp"

using json = nlohmann::json;

class Genome
{
public:
    /**
     * @brief Constructeur par défaut pour la classe Genome.
     *
     * Initialise un objet Genome avec des valeurs par défaut :
     * - genome_id est initialisé à 0.
     * - num_inputs est réglé sur 0.
     * - num_outputs est réglé sur 0.
     */
    Genome();

    /**
     * @brief Construit un nouvel objet Génome.
     *
     * @param id L’identifiant unique du génome.
     * @param num_inputs Nombre de nœuds d’entrée dans le génome.
     * @param num_outputs Le nombre de nœuds de sortie dans le génome.
     */
    Genome(int id, int num_inputs, int num_outputs);

    static int last_id;

    bool would_create_cycle(int input_id, int output_id) const;

    bool operator==(const Genome &other) const;
    
    friend std::ostream& operator<<(std::ostream& os, const Genome& genome) {
    os << "Genome(ID: " << genome.get_genome_id() << ")";
    return os;
}

    // Méthodes statiques pour créer un génome
    static Genome create_genome(int id, int num_inputs, int num_outputs, int num_hidden_neurons, RNG &rng);

    static Genome create_genome_div(int id, int num_inputs, int num_outputs, int num_hidden_neurons, RNG &rng);

    static Genome create_diverse_genome(int id, int num_inputs, int num_outputs, int max_hidden_neurons, RNG &rng);

    static Genome create_diverse_genome_unique(int num_inputs, int num_outputs, int max_hidden_neurons, RNG &rng);

    static Genome create_minimal_genome(int num_inputs, int num_outputs, RNG &rng);



    double compute_distance(const Genome &other, const NeatConfig &config) const;

    /**
     * @brief Obtenir le nombre d’entrées dans le génome.
     *
     * Cette fonction renvoie le nombre de nœuds d’entrée dans le génome.
     *
     * @return int Le nombre de nœuds d’entrée.
     */
    int get_num_inputs() const;

    /**
     * @brief Obtenir le nombre de sorties dans le génome.
     *
     * Cette fonction renvoie le nombre de nœuds de sortie dans le génome.
     *
     * @return int Le nombre de nœuds de sortie.
     */
    int get_num_outputs() const;

    /**
     * @brief Récupère l’ID du génome.
     *
     * @return int L’identifiant du génome.
     */
    int get_genome_id() const;

    /**
     * @brief Récupère les neurones du génome.
     *
     * @return std::vector<neat::NeuronGene> Les neurones du génome.
     */
    std::vector<neat::NeuronGene> get_neurons() const;

    /**
     * @brief Récupère les liens du génome.
     *
     * @return std::vector<neat::LinkGene> Les liens du génome.
     */
    std::vector<neat::LinkGene> get_links() const;

    std::vector<neat::NeuronGene>& get_neurons();
    std::vector<neat::LinkGene>& get_links();

    /**
     * @brief Ajoute un neurone au génome.
     *
     * Cette fonction ajoute un gène de neurone donné à la liste des neurones du génome.
     *
     * @param neuron Le gène neurone à ajouter.
     */
    void add_neuron(const neat::NeuronGene &neuron);

    /**
     * @brief Ajoute un lien donné à la liste des liens dans le génome.
     *
     * TCette fonction ajoute un gène de lien donné à la liste des liens du génome.
     *
     * @param link Le gène de lien à ajouter.
     */
    void add_link(const neat::LinkGene &link);

    // Recherche de neurones et de liens
    std::optional<neat::NeuronGene> find_neuron(int neuron_id) const;
    std::optional<neat::LinkGene> find_link(neat::LinkId link_id) const;

    /**
     * @brief Génère le prochain ID de neurone unique.
     *
     * Cette fonction itère à travers les neurones existants dans le génome et trouve l’ID maximum du neurone.
     * Il renvoie ensuite un nouvel identifiant supérieur d’un au maximum actuel, garantissant que chaque neurone
     * possède un identifiant unique.
     *
     * @return int Le prochain ID unique de neurone.
     */
    int generate_next_neuron_id();

    // Génération de vecteurs d'IDs pour les neurones d'entrée et de sortie
    std::vector<int> make_input_ids() const;
    std::vector<int> make_output_ids() const;

    /**
     * @brief Crée un nouveau lien avec les identifiants de neurones spécifiés.
     *
     * Cette méthode crée un nouveau lien avec les identifiants de neurones spécifiés, un poids
     * initialisé aléatoirement et activé par défaut.
     *
     * @param input_id L'identifiant du neurone d'entrée pour le lien.
     * @param output_id L'identifiant du neurone de sortie pour le lien.
     * @return neat::LinkGene Une structure LinkGene représentant le lien nouvellement créé.
     */
    neat::LinkGene create_link(int input_id, int output_id, RNG &rng);

    neat::LinkGene create_link_div(int input_id, int output_id, RNG &rng);

    neat::LinkGene create_link_div_with_inov(int input_id, int output_id, RNG &rng);

    /**
     * @brief Crée un nouveau neurone avec l'identifiant de neurone spécifié.
     *
     * Cette méthode crée un nouveau neurone avec l'identifiant de neurone spécifié, un biais
     * initialisé à 0.0 et une fonction d'activation par défaut (Sigmoid).
     *
     * @param neuron_id L'identifiant unique du neurone à créer.
     * @return Une structure NeuronGene représentant le neurone nouvellement créé.
     */
    neat::NeuronGene create_neuron(int neuron_id);

    friend void to_json(json& json, const Genome& genome);
    friend void from_json(const json& json, Genome& genome);

private:
    int genome_id;
    int num_inputs;
    int num_outputs;

    // Vecteurs de neurones et de liens dans le génome
    std::vector<neat::NeuronGene> neurons;
    std::vector<neat::LinkGene> links;
};

namespace std {
    template <>
    struct hash<Genome> {
        size_t operator()(const Genome& genome) const {
            return std::hash<int>()(genome.get_genome_id());
        }
    };
}

void to_json(json& json, const Genome& genome);
void from_json(const json& json, Genome& genome);




#endif // GENOME_H
