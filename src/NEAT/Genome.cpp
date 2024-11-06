#include "Genome.h"
#include "neat.h"  // Inclure neat.h pour les définitions complètes
#include <optional>  // Inclure <optional> pour utiliser std::optional


/**
 * @brief Constructeur par défaut pour la classe Genome.
 * 
 * Initialise un objet Genome avec des valeurs par défaut :
 * - genome_id est initialisé à 0.
 * - num_inputs est réglé sur 0.
 * - num_outputs est réglé sur 0.
 */
Genome::Genome() : genome_id(0), num_inputs(0), num_outputs(0) {}

/**
 * @brief Construit un nouvel objet Génome.
 * 
 * @param id L’identifiant unique du génome.
 * @param num_inputs Nombre de nœuds d’entrée dans le génome.
 * @param num_outputs Le nombre de nœuds de sortie dans le génome.
 */
Genome::Genome(int id, int num_inputs, int num_outputs)
    : genome_id(id), num_inputs(num_inputs), num_outputs(num_outputs) {}

/**
 * @brief Obtenir le nombre d’entrées dans le génome.
 * 
 * Cette fonction renvoie le nombre de nœuds d’entrée dans le génome.
 * 
 * @return int Le nombre de nœuds d’entrée.
 */
int Genome::get_num_inputs() const {
    return num_inputs;  // Retourne le nombre d'entrées
}

/**
 * @brief Obtenir le nombre de sorties dans le génome.
 * 
 * Cette fonction renvoie le nombre de nœuds de sortie dans le génome.
 * 
 * @return int Le nombre de nœuds de sortie.
 */
int Genome::get_num_outputs() const {
    return num_outputs;  // Retourne le nombre de sorties
}

/**
 * @brief Récupère l’ID du génome.
 * 
 * @return int L’identifiant du génome.
 */
int Genome::get_genome_id() const {
    return genome_id;  // Retourne l'ID du génome
}

/**
 * @brief Génère le prochain ID de neurone unique.
 *
 * Cette fonction itère à travers les neurones existants dans le génome et trouve l’ID maximum du neurone.
 * Il renvoie ensuite un nouvel identifiant supérieur d’un au maximum actuel, garantissant que chaque neurone
 * possède un identifiant unique.
 *
 * @return int Le prochain ID unique de neurone.
 */
int Genome::generate_next_neuron_id() {
    int max_id = 0;
    for (const auto& neuron : neurons) {
        if (neuron.neuron_id > max_id) {
            max_id = neuron.neuron_id;
        }
    }
    return max_id + 1;
}


/**
 * @brief Ajoute un neurone au génome.
 * 
 * Cette fonction ajoute un gène de neurone donné à la liste des neurones du génome.
 * 
 * @param neuron Le gène neurone à ajouter.
 */
void Genome::add_neuron(const neat::NeuronGene &neuron) {
    neurons.push_back(neuron);
}

/**
 * @brief Ajoute un lien donné à la liste des liens dans le génome.
 * 
 * TCette fonction ajoute un gène de lien donné à la liste des liens du génome.
 * 
 * @param link Le gène de lien à ajouter.
 */
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

/**
 * @brief Trouve un lien dans le génome à partir de son ID de lien.
 *
 * Cette fonction parcourt la liste des liens dans le génome pour trouver un lien
 * correspondant à l'ID de lien donné. Si un lien correspondant est trouvé, il est retourné.
 * Si aucun lien correspondant n'est trouvé, une option vide est retournée.
 *
 * @param link_id L'ID du lien à trouver.
 * @return std::optional<neat::LinkGene> Le lien s'il est trouvé, sinon une option vide.
 */
std::optional<neat::LinkGene> Genome::find_link(neat::LinkId link_id) const {
    for (const auto &link : links) {
        if (link.link_id.input_id == link_id.input_id && link.link_id.output_id == link_id.output_id) {
            return link;  // Retourne le lien s'il est trouvé
        }
    }
    return std::nullopt;  // Retourne un optional vide si non trouvé
}

/**
 * @brief Génère un vecteur contenant les identifiants des nœuds d’entrée.
 *
 * Cette fonction crée un vecteur d’entiers où chaque entier représente
 * l’ID d’un nœud d’entrée. Les ID sont générés séquentiellement à partir de 0
 * jusqu’au nombre de nœuds d’entrée spécifié par la variable membre `num_inputs’.
 *
 * @return Un vecteur d’entiers représentant les identifiants des nœuds d’entrée.
 */
std::vector<int> Genome::make_input_ids() const {
    std::vector<int> input_ids;
    for (int i = 0; i < num_inputs; i++) {
        input_ids.push_back(i);  // Ajoute les IDs des entrées
    }
    return input_ids;
}

/**
 * @brief Génère un vecteur contenant les identifiants des nœuds de sortie.
 *
 * Cette fonction crée un vecteur d’entiers représentant les identifiants des nœuds de sortie
 * dans le génome. Les ID sont calculés en fonction du nombre de nœuds d’entrée et des 
 * nombre de nœuds de sortie.
 *
 * @return Un vecteur d’entiers contenant les identifiants des nœuds de sortie.
 */
std::vector<int> Genome::make_output_ids() const {
    std::vector<int> output_ids;
    for (int i = 0; i < num_outputs; i++) {
        output_ids.push_back(num_inputs + i);  // Ajoute les IDs des sorties
    }
    return output_ids;
}

