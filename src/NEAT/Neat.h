// Neat.h
#ifndef NEAT_H
#define NEAT_H

#include "Neat.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "Activation.h"
#include "GenomeIndexer.h"
#include "NeatConfig.h"
#include <memory>
#include "../external/json.hpp"

using json = nlohmann::json;

class Genome;



namespace neat
{
    
    

    // Structure d'un neurone dans le génome
    struct NeuronGene
    {
        int neuron_id;
        double bias;
        Activation activation; // Enum ou fonction d'activation

        bool is_hidden(int neuron_id, const NeatConfig &config) const
        {
            return neuron_id >= config.num_inputs + config.num_outputs;
        }
    };

    // Identifiants de la connexion entre deux neurones
    struct LinkId
    {
        int input_id;
        int output_id;

        // Définir l'opérateur == pour comparer deux LinkId
        bool operator==(const LinkId &other) const
        {
            return input_id == other.input_id && output_id == other.output_id;
        }
    };

    // Spécialisation de std::hash pour LinkId
    struct LinkIdHash
    {
        std::size_t operator()(const LinkId &link_id) const
        {
            return std::hash<int>()(link_id.input_id) ^ std::hash<int>()(link_id.output_id);
        }
    };

    // Structure représentant une connexion entre deux neurones
    struct LinkGene
    {
        LinkId link_id;  // Connexion entre les neurones
        double weight;   // Poids de la connexion
        bool is_enabled; // Si la connexion est active

        // Définir l'opérateur == pour comparer deux LinkGene
        bool operator==(const LinkGene &other) const
        {
            return link_id == other.link_id && weight == other.weight && is_enabled == other.is_enabled;
        }

        bool has_neuron(const NeuronGene &neuron) const
        {
            return link_id.input_id == neuron.neuron_id || link_id.output_id == neuron.neuron_id;
        }
    };

    void to_json(json& json, const neat::LinkGene& link);
    void from_json(const json& json, neat::LinkGene& link);

    void to_json(json& json, const neat::NeuronGene& neuron);
    void from_json(const json& json, neat::NeuronGene& neuron);

    // Structure pour représenter un individu
    struct Individual
{
    std::shared_ptr<Genome> genome;  // Utilise std::shared_ptr pour gérer le cycle de vie
    bool fitness_computed;
    double fitness;

    Individual()
        : genome(nullptr), fitness_computed(false), fitness(0.0) {}

    Individual(std::shared_ptr<Genome> genome)
        : genome(std::move(genome)), fitness_computed(false), fitness(0.0) {}
};

    struct DoubleConfig
    {
        double init_mean = 0.0;
        double init_stdev = 1.0;
        double min_value = -20.0;
        double max_value = 20.0;
        double mutation_rate = 0.2;
        double mutate_power = 1.2;
        double replace_rate = 0.05;
    };

    // Classe Neat contenant les méthodes d'évolution
    class Neat
    {
    public:
        /**
         * @brief Effectue un croisement entre deux objets NeuronGene.
         *
         * Cette fonction prend deux objets NeuronGene avec le même neuron_id et
         * effectue une opération de croisement pour produire un nouveau NeuronGène. Le biais et
         * l’activation du NeuronGene résultant sont choisis au hasard parmi les
         * valeurs correspondantes des objets NeuronGene d’entrée.
         *
         * @param a Le premier parent NeuronGene.
         * @param b Le deuxième parent NeuronGene.
         * @return Un nouveau NeuronGene résultant du croisement des NeuronGènes d’entrée.
         * @throws std::assert si le neuron_id de l’entrée NeuronGènes n’est pas le même.
         */
        NeuronGene crossover_neuron(const NeuronGene &a, const NeuronGene &b);

        /**
         * @brief Effectue un croisement entre deux objets LinkGene.
         *
         * Cette fonction prend deux objets LinkGene, ‘a’, et ‘b’, et effectue une opération de croisement.
         * pour produire un nouveau LinkGene. Le croisement se fait en choisissant au hasard le poids et
         * statut activé à partir de l’un ou de l’autre.
         *
         * @param a Le premier parent LinkGene.
         * @param b Le deuxième parent LinkGene.
         * @return Un nouveau LinkGene résultant de la jonction des deux.
         *
         * @pre L’identifiant d’entrée et l’identifiant de sortie de « a » et de « b » doivent être les mêmes.
         */
        LinkGene crossover_link(const LinkGene &a, const LinkGene &b);

        /**
         * @brief Effectue un croisement entre deux individus pour produire un génome de progéniture.
         *
         * Cette fonction prend deux individus parents, un dominant et un récessif, et combine leurs génomes
         * pour produire un génome de descendance. La descendance hérite des neurones et des liens du parent dominant,
         * et, dans la mesure du possible, les combine avec des neurones correspondants et des liens provenant du parent récessif.
         *
         * @param dominant Le parent dominant dont le génome contribuera principalement à la descendance.
         * @param recessive Le parent récessif dont le génome contribuera de façon secondaire à la descendance.
         * @param child_genome_id L’identifiant unique du génome de la progéniture.
         * @return Genome Le génome de la descendance après un croisement.
         */
        Genome crossover(const Individual &dominant, const Individual &recessive, int child_genome_id);

        Genome alt_crossover(const std::shared_ptr<Genome>& dominant, 
                       const std::shared_ptr<Genome>& recessive, 
                       int child_genome_id);

    private:
        GenomeIndexer m_genome_indexer;
    };

    /**
     * @brief Effectue une recherche en profondeur (DFS) sur un graphe à partir d'un neurone donné.
     *
     * Cette fonction explore récursivement tous les neurones accessibles à partir du neurone spécifié
     * en utilisant l'algorithme de recherche en profondeur (DFS). Elle marque chaque neurone visité
     * pour éviter les boucles infinies et les visites répétées.
     *
     * @param neuron_id L'identifiant du neurone de départ pour la recherche en profondeur.
     * @param graph Une référence constante à une map non ordonnée représentant le graphe, où la clé est
     *              l'identifiant d'un neurone et la valeur est un vecteur d'identifiants de neurones voisins.
     * @param visited Une référence à un ensemble non ordonné d'identifiants de neurones déjà visités.
     *
     * @note Si l'identifiant du neurone de départ n'existe pas dans le graphe, un message d'erreur est affiché
     *       et la fonction retourne immédiatement.
     */
    void dfs(int neuron_id, const std::unordered_map<int, std::vector<int>> &graph, std::unordered_set<int> &visited);

    /**
     * @brief Limite une valeur donnée dans la plage spécifiée par DoubleConfig.
     *
     * Cette fonction prend une valeur de type double et s'assure qu'elle se trouve
     * dans les valeurs minimum et maximum définies par l'objet DoubleConfig.
     * Si la valeur est inférieure au minimum, elle retourne le minimum.
     * Si la valeur est supérieure au maximum, elle retourne le maximum.
     * Sinon, elle retourne la valeur elle-même.
     *
     * @param x La valeur de type double à limiter.
     * @return La valeur limitée dans la plage [config.min_value, config.max_value].
     */
    double clamp(double x);
    

} // namespace neat

#endif // NEAT_H
