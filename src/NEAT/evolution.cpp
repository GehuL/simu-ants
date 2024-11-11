#include "evolution.h"
#include "NeatConfig.h"

using namespace neat;

GeneticTrainer::GeneticTrainer(std::function<void> fitness) :
m_population((NeatConfig) {.num_inputs=simu::AntIA::inputCount(), 
.num_outputs=simu::AntIA::outputCount()}, RNG())
{

}

const std::vector<neat::Individual>& GeneticTrainer::train()
{
    for(auto& ind : m_population.get_individuals())
    {
        // ind.fitness = m_fitness();
        // ind.fitness_computed=true;
    }
    auto new_generation = population.reproduce();
    population.set_individuals(new_generation);  // Remplace la population par la nouvelle génération
    m_genCount++;
    return new_generation;
} 