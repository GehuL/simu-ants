#include "../engine/world.h"
#include "../NEAT/Population.h"
#include "../NEAT/evolution.h"

using namespace simu;

class Evolution : public WorldListener
{
    public:
        Evolution(int evolutionPeriod = 5000) : m_evolutionPeriod(evolutionPeriod) {};

        void onInit() override
        {
            auto ants = m_world.spawnEntities<AntIA>(100);
            m_trainer.init(ants);
        };

        void onUpdate() override
        {
            if(m_evolutionTick >= m_evolutionPeriod)
            {
                m_tickCrossover = 0;
                m_world.init(); // Clear all
                
                auto genomes = m_trainer.train();
                for(auto& genome : genomes)
                {
                    m_world.spawnEntities<AntIA>(genome);
                }
            }else
            {
                m_evolutionTick++;
            }
        };

        void onUnload() override
        {

        };

    private:
        World& m_world = getWorld();
        const int m_evolutionPeriod = 5000;
        int m_evolutionTick; 
        GeneticTrainer m_trainer;
};

int main()
{
    RNG rng;
    NeatConfig config {.num_inputs=3, .num_outputs=1};

    World& world = getWorld();
    world.setListener(std::make_shared<Evolution>(rng, config));

    return world.run(800, 800, "Maze evolution");
}