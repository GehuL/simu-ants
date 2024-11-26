#ifndef MUTATOR_H
#define MUTATOR_H

#include "Neat.h"
#include "Genome.h"
#include "RNG.h"
#include "NeatConfig.h"

class Mutator
{
public:
    // Méthode pour appliquer différentes mutations sur un génome
    static void mutate(Genome &genome, const NeatConfig &config, RNG &rng);

    // Mutations spécifiques

    /**
     * @brief Modifie le génome donné en modifiant le poids d’un lien aléatoire.
     *
     * Cette fonction choisit un lien aléatoire dans le génome et modifie son poids en fonction de la configuration NEAT.
     * Elle utilise un générateur de nombres aléatoires pour déterminer si le poids est muté en ajoutant ou en soustrayant une valeur aléatoire.
     *
     * @param genome Le génome à muter.
     * @param config La configuration NEAT utilisée pour la mutation.
     * @param rng Une référence à un générateur de nombres aléatoires.
     *
     * @details La fonction effectue les étapes suivantes :
     * - Sélectionne un lien aléatoire dans le génome.
     * - Génère un delta aléatoire en utilisant une distribution gaussienne avec une moyenne de 0 et un écart type défini par la puissance de mutation.
     * - Ajoute le delta à la valeur actuelle du poids du lien.
     * - Limite la nouvelle valeur du poids dans la plage définie par la configuration NEAT.
     *
     * @note La fonction ne modifie pas le poids du lien si le lien est désactivé.
     */
    static void mutate_link_weight(Genome &genome, const NeatConfig &config, RNG &rng);

    /**
     * @brief Modifie le génome donné en modifiant le biais d’un neurone aléatoire.
     *
     * Cette fonction choisit un neurone aléatoire dans le génome et modifie son biais en fonction de la configuration NEAT.
     * Elle utilise un générateur de nombres aléatoires pour déterminer si le biais est muté en ajoutant ou en soustrayant une valeur aléatoire.
     *
     * @param genome Le génome à muter.
     * @param config La configuration NEAT utilisée pour la mutation.
     * @param rng Une référence à un générateur de nombres aléatoires.
     *
     * @details La fonction effectue les étapes suivantes :
     * - Sélectionne un neurone aléatoire dans le génome.
     * - Génère un delta aléatoire en utilisant une distribution gaussienne avec une moyenne de 0 et un écart type défini par la puissance de mutation.
     * - Ajoute le delta au biais actuel du neurone.
     * - Limite la nouvelle valeur du biais dans la plage définie par la configuration NEAT.
     */
    static void mutate_neuron_bias(Genome &genome, const NeatConfig &config, RNG &rng);

    /**
     * @brief Modifie le génome donné en ajoutant un nouveau lien entre les neurones.
     *
     * Cette fonction tente d’ajouter un nouveau lien entre deux neurones choisis au hasard
     * dans le génome fourni. Il garantit que la liaison n’existe pas déjà et
     * que l’ajout du lien ne crée pas de cycle dans le réseau.
     *
     * @param genome Le génome à muter.
     *
     * @détails La fonction effectue les étapes suivantes :
     * - Choisit une entrée aléatoire ou un neurone caché.
     * - Choisit une sortie aléatoire ou un neurone caché.
     * - Si des neurones valides ne sont pas trouvés, la fonction revient sans faire de changements.
     * - Vérifie si le lien existe déjà dans le génome.
     * - Si le lien existe et est désactivé, il réactive le lien.
     * - Si le lien n’existe pas, il vérifie si l’ajout du lien créerait un cycle.
     * - Si l’ajout du lien ne crée pas de cycle, il crée et ajoute le nouveau lien au génome.
     */
    static void mutate_add_link(Genome &genome);

    /**
     * @brief Modifie le génome donné en supprimant un lien non essentiel.
     *
     * Cette fonction identifie et supprime un lien du génome qui n’est pas
     * essentiels pour la fonctionnalité du réseau. Les liens essentiels sont ceux qui
     * connecter les neurones d’entrée aux neurones cachés, les neurones cachés aux neurones de sortie,
     * et liens entre les neurones cachés.
     *
     * @param genome Le génome à muter.
     */
    static void mutate_remove_link(Genome &genome);

    /**
     * @brief Modifie le génome donné en ajoutant un nouveau neurone.
     *
     * Cette fonction effectue les étapes suivantes :
     * 1. Vérifie si le génome a des liens. Sinon, il revient immédiatement.
     * 2. Sélectionne une liaison aléatoire à partir du génome pour le fractionnement.
     * 3. Désactive le lien sélectionné.
     * 4. Supprime le lien désactivé du génome.
     * 5. Crée un nouveau neurone en utilisant le NeuronMutator.
     * 6. Génère un nouvel ID de neurone et ajoute le nouveau neurone au génome.
     * 7. Ajoute un nouveau lien du neurone d’entrée du lien de division au nouveau neurone avec un poids de 1.0.
     * 8. Ajoute un nouveau lien du nouveau neurone au neurone de sortie du lien divisé avec le poids du lien d’origine.
     *
     * @param genome Le génome à muter en ajoutant un nouveau neurone.
     */
    static void mutate_add_neuron(Genome &genome);

    /**
     * @brief Modifie le génome donné en supprimant un neurone caché.
     *
     * Cette fonction supprime un neurone caché du génome s’il reste au moins deux neurones cachés.
     * Il compte d’abord le nombre de neurones cachés et renvoie s’il y en a moins de deux.
     * Ensuite, il sélectionne au hasard un neurone caché, supprime tous les liens qui lui sont associés et enfin supprime le neurone lui-même.
     *
     * @param genome Le génome à muter.
     */
    static void mutate_remove_neuron(Genome &genome);
};

// Méthodes utilitaires pour choisir des neurones aléatoires

/**
 * @brief Sélectionne une entrée aléatoire ou un neurone caché dans une liste de neurones.
 *
 * Cette fonction itère à travers une liste de neurones et sélectionne ceux qui sont soit
 * neurones d’entrée (ID entre 0 et num_inputs - 1) ou neurones cachés (ID supérieur à
 * num_inputs + num_outputs - 1). Il sélectionne ensuite de façon aléatoire l’un de ces neurones valides.
 *
 * @param neurons Un vecteur d’objets NeuronGene représentant les neurones.
 * @return L’identifiant d’un neurone caché ou d’une entrée choisie au hasard. Si aucun neurone valide n’est trouvé,
 *   renvoie -1.
 */
static int choose_random_input_or_hidden_neuron(const std::vector<neat::NeuronGene> &neurons);

/**
 * @brief Sélectionne une sortie aléatoire ou un neurone caché dans une liste de neurones.
 *
 * Cette fonction itère à travers une liste de neurones et sélectionne ceux dont les ID
 * se situent dans la gamme des neurones de sortie, telle que définie par le NeatConfig. Il
 * sélectionne au hasard un de ces neurones valides et renvoie son identifiant.
 *
 * @param neurons Un vecteur d’objets NeuronGene représentant les neurones à choisir.
 * @return L’identifiant d’un neurone valide choisi au hasard, ou -1 si aucun neurone valide n’est trouvé.
 */
static int choose_random_output_or_hidden_neuron(const std::vector<neat::NeuronGene> &neurons);

// Méthodes pour choisir des neurones cachés aléatoires

/**
 * @brief Choisit un neurone caché aléatoire dans une liste de neurones.
 *
 * Cette fonction itère à travers une liste de neurones et identifie les neurones cachés
 * en fonction de leur neuron_id. Un neurone caché est défini comme ayant un ID supérieur à
 * ou égal à la somme du nombre d’entrées et de sorties. Il sélectionne ensuite au hasard
 * un de ces neurones cachés et renvoie un itérateur à celui-ci.
 *
 * @param neurons Référence à un vecteur d’objets NeuronGene représentant les neurones.
 * @return Un itérateur à un neurone caché choisi au hasard.
 * @throws std::out_of_range Si aucun neurone caché n’est disponible dans la liste.
 */
std::vector<neat::NeuronGene>::const_iterator choose_random_hidden(std::vector<neat::NeuronGene> &neurons);

// Méthode pour vérifier si un cycle serait créé par l'ajout d'un lien

/**
 * @brief Vérifie si l’ajout d’un lien entre deux neurones créerait un cycle dans le réseau.
 *
 * Cette fonction utilise un algorithme de recherche en profondeur pour déterminer si un cycle serait créé.
 * en ajoutant un lien entre le neurone avec l’identifiant `input_id` et le neurone avec l'«output_id`. Il traverse la
 * réseau à partir du neurone `output_id`et vérifie s’il peut atteindre le neurone `input_id`.
 *
 * @param links Un vecteur de `neat::LinkGene` représentant les liens existants dans le réseau.
 * @param input_id L’ID du neurone d’entrée du lien à ajouter.
 * @param output_id L’ID du neurone de sortie du lien à ajouter.
 * @return true si l’ajout du lien créerait un cycle, false sinon.
 */
bool would_create_cycle(const std::vector<neat::LinkGene> &links, int input_id, int output_id);

/**
 * @brief Génère une nouvelle valeur basée sur une distribution gaussienne.
 *
 * Cette fonction crée une nouvelle valeur aléatoire en utilisant une distribution gaussienne (normale)
 * avec une moyenne et un écart-type spécifiés. La valeur est ensuite serrée pour assurer
 * il se situe dans une fourchette valable.
 *
 * @return Un double représentant la nouvelle valeur clampée générée à partir de la distribution gaussienne.
 */
double new_value();

/**
 * @brief Fait muter une valeur donnée en ajoutant un delta généré à partir d'une distribution gaussienne.
 *
 * Cette fonction génère un delta en utilisant une distribution gaussienne avec une moyenne de 0 et un écart type
 * défini par la puissance de mutation dans la configuration. Le delta est ensuite limité et ajouté à la valeur
 * d'entrée, et le résultat est à nouveau limité avant d'être retourné.
 *
 * @param value La valeur initiale à faire muter.
 * @return La valeur mutée après ajout du delta limité.
 */
double mutate_delta(double value);

#endif // MUTATOR_H
