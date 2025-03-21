#ifndef POPULATION_H
#define POPULATION_H

#include "Mutator.h"
#include "rng.h"
#include "ComputeFitness.h"
#include "Genome.h"
#include "NeatConfig.h"
#include "species.h"
#include <vector>
#include <algorithm>
#include <cmath>

class Population
{
public:

   
   /**
    * @brief Constructeur de la classe Population.
    *
    * Ce constructeur initialise une population de génomes en utilisant les paramètres de configuration
    * NEAT spécifiés et un générateur de nombres aléatoires. Chaque génome est initialisé avec des neurones
    * d'entrée et de sortie, ainsi qu'un nombre aléatoire de neurones cachés.
    *
    * @param config La configuration NEAT utilisée pour initialiser la population.
    * @param rng Une référence à un générateur de nombres aléatoires.
    */
   Population(NeatConfig config, RNG &rng);

   /**
    * @brief Retourne une référence à un vecteur contenant les individus de la population.
    *
    * @return La référence à un vecteur d'individus.
    */
   std::vector<neat::Individual> &get_individuals();

   /**
    * @brief Génère le prochain identifiant unique de génome et l'incrémente.
    *
    * Cette méthode génère le prochain identifiant unique de génome en utilisant la variable
    * next_genome_id, puis incrémente cette variable pour le prochain appel.
    *
    * @return L'identifiant unique du prochain génome.
    */
   int generate_next_genome_id();

   int generate_genome_id();

   /**
    * @brief Mute le génome en ajoutant ou en supprimant des liens et des neurones, et en modifiant les poids et les biais.
    *
    * Cette méthode mute le génome en effectuant les opérations suivantes avec des probabilités définies par la configuration NEAT :
    * - Ajouter un lien avec la probabilité définie par `config.probability_add_link`.
    * - Supprimer un lien avec la probabilité définie par  `config.probability_remove_link`.
    * - Ajouter un neurone avec la probabilité définie par `config.probability_add_neuron`.
    * - Supprimer un neurone avec la probabilité définie par `config.probability_remove_neuron`.
    * - Faire muter le poids d'un lien aléatoire avec la probabilité définie par `config.probability_mutate_link_weight`.
    * - Faire muter le biais d'un neurone aléatoire avec la probabilité définie par `config.probability_mutate_neuron_bias`.
    *
    * @param genome Le génome à muter.
    */
   void mutate(Genome &genome);

   /**
    * @brief Permet de reproduire la population actuelle en fonction de la fitness, de sélectionner les parents parmi les meilleurs individus
    *
    * Cette méthode permet de reproduire la population actuelle en fonction de la fitness des individus. Les individus sont triés par ordre de fitness
    * décroissante, puis les parents sont sélectionnés parmi les meilleurs individus en fonction d'un seuil de survie défini par la configuration NEAT.
    * Les parents sont ensuite croisés pour créer de nouveaux individus, qui sont ensuite mutés pour introduire de la diversité.
    *
    * @return std::vector<neat::Individual> Un vecteur contenant les nouveaux individus de la génération suivante.
    */
   std::vector<neat::Individual> reproduce();

   std::vector<neat::Individual> reproduce_from_genomes(const std::vector<std::shared_ptr<Genome>>& genomes);

   std::vector<neat::Individual> reproduce_from_genomes_with_fitness(
       const std::vector<std::shared_ptr<Genome>>& genomes,
       const std::vector<double>& fitnesses
   );

   std::vector<neat::Individual> reproduce_from_genome_roulette(
       const std::vector<std::shared_ptr<Genome>>& genomes,
       const std::vector<double>& fitnesses
   );

   std::vector<neat::Individual> reproduce_from_genome_roulette_negative(
       const std::vector<std::shared_ptr<Genome>>& genomes,
       const std::vector<double>& fitnesses
   );

 std::vector<neat::Individual> reproduce_with_speciation(
    const std::vector<Species>& species_list,
    const std::unordered_map<Genome, double>& fitness_map

);


   /**
    * @brief Trie les individus par fitness en ordre décroissant.
    *
    * Cette méthode trie les individus par fitness en ordre décroissant, du plus grand au plus petit.
    * Elle retourne un nouveau vecteur d'individus trié par fitness.
    *
    * @param individuals Un vecteur d'individus à trier.
    * @return Un nouveau vecteur d'individus trié par fitness.
    */
   std::vector<neat::Individual> sort_individuals_by_fitness(const std::vector<neat::Individual> &individuals);

   /**
    * @brief Met à jour le meilleur individu de la population.
    *
    * Cette méthode met à jour le meilleur individu de la population en cherchant l'individu avec la meilleure fitness.
    * Si un meilleur individu est trouvé, la variable best_individual est mise à jour avec cet individu.
    *
    */
   void update_best();


/**
 * @brief Remplace la population actuelle par une nouvelle génération d'individus.
 *
 * Cette méthode prend en entrée un vecteur de nouveaux individus, 
 * et met à jour la population en remplaçant les anciens individus 
 * par les nouveaux tout en conservant les informations nécessaires.
 *
 * @param new_generation Le vecteur contenant les nouveaux individus.
 */
   void replace_population(std::vector<neat::Individual> new_generation);

    /**
     * @brief Efface toutes les espèces de la population.
     *
     * Cette méthode efface toutes les espèces de la population en vidant la liste des espèces.
     * Elle est appelée à la fin de chaque génération pour nettoyer les données de la génération précédente.
     */

    void clear_species();

    void create_new_species(std::shared_ptr<Genome> representative);

    std::vector<Species> &get_species_list();

    int generate_next_species_id();

    void update_species_representatives();

    

   
private:
   NeatConfig config;
   RNG &rng;
   int next_genome_id;
   static int species_id_counter;
   std::vector<neat::Individual> individuals;
   neat::Individual best_individual;
   std::vector<Species> species_list;
};

#endif // POPULATION_H
