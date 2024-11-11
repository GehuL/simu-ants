#ifndef __EVOLUTION_H__
#define __EVOLUTION_H__

#include <vector>
#include "../engine/ant.h"
#include "Genome.h"
#include "Population.h"

namespace neat
{
    /**
     * Interface permettant d'implémenter le processus d'evolution génétique 
     */
    template<typename T>
    class AbstractGenetic
    {
        public:
            virtual ~AbstractGenetic() = 0;

            /**
             * @brief Initialise une population de N éléments
             */
            virtual std::vector<T>& initialize() = 0;

            /**
             * @brief Evalue un individu pour lui donner un score
             * @return Le score 
             */
            virtual double evaluate(const T& individu) = 0;

            /**
             * @brief Selectionne N éléments selon les valeurs retournées par evaluate()
             * @brief La population à selectionner
             * @return La liste des individus sélectionnés
             */
            virtual std::vector<T>& select(const std::vector<T>& pop) = 0;

            /**
             * @brief Croise les gènes issuent des parents choisis par la méthode select()
             * @return Renvoie les enfants
             */
            virtual std::vector<T>& crossover(const std::vector<T>& pop) = 0;

            /**
             * @brief Applique une mutation des gènes des enfant issus de la méthode crossover()
             * @return Les enfants mutés
             */
            virtual std::vector<T>& mutate(const std::vector<T>& pop) = 0;
    };

    // template<typename T, typename = std::enable_if_t<std::is_base_of<AbstractGenetic, T>::value>>
    // class GeneticTrainer
    // {
    //     public:
    //         GeneticTrainer() {};
    //         GeneticTrainer(const std::vector<T>& population) : m_population(population) {};
            
    //         virtual ~GeneticTrainer() {};

    //         const std::vector<T> train() {};

    //         int getGenerationCount() { return m_genCount; };
            
    //         /**
    //          * Renvoie la population actuelle
    //          */
    //         const std::vector<T> getPopulation() { return m_population; };

    //     private:
    //         int m_genCount;
    //         T m_genetic;
    //         std::vector<T> m_population;
    // };

    class GeneticTrainer
    {
        public:
            GeneticTrainer(std::function<double(simu::AntIA)> fitness);

            /**
             * @brief Renvoie la nouvelle génération de génome
             */
            const std::vector<Individual>& train();

            int generationCount() const { return m_genCount; }

        private:
            std::function<double(simu::AntIA)> m_fitness;
            Population m_population;
            int m_genCount;
    };
}

#endif
