#ifndef __GENETIC_ANT_H__
#define __GENETIC_ANT_H__

#include "ant.h"
#include "../NEAT/evolution.h"
#include "../NEAT/NeuronMutator.h"

namespace simu
{
    class AntGenetic : public AbstractGenetic<Ant>
    {
        public:
            std::vector<T>& initialize() override
            {
                
            }

            std::vector<T>& evaluate(T& individu) override
            {

            }

            std::vector<T>& select() override
            {

            }

            std::vector<T>& crossover() override
            {

            }
            
            std::vector<T>& mutate()
            {

            }
    }
}

#endif