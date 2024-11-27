#include "engine/world.h"
#include "engine/ant.h"
#include "NEAT/population.h"

using namespace simu;

RNG rng;

class Scene: public WorldListener
{
    public:
        Scene(): mPop((NeatConfig) {}, rng) {};

        void onInit() override
        {
            getWorld().getGrid().fromImage("rsc/maze.png");
            m_ants = getWorld().spawnEntities<AntIA>(10);
        };

        void onUnload() override
        {

        };
        
        void onUpdate() override
        {
            std::vector<std::shared_ptr<Genome>> genomes;
            for(auto& ant: m_ants)
            {
                if(!ant.expired())
                {
                    auto genome = std::make_shared<Genome>(ant.lock()->getGenome());
                    genomes.push_back(genome);
                }
            }

            m_ants.clear();
            auto newlies = mPop.reproduce_from_genomes(genomes);
            for(auto& ant: newlies)
            {
                getWorld().spawnEntity<AntIA>(newlies);
            }
        };

    private:
        std::vector<std::weak_ptr<AntIA>> m_ants;
        Population mPop;
};

int main(void)
{   
    simu::World& world = simu::getWorld(); 
    world.setListener(std::make_shared<Scene>());
    return world.run(800, 800, "ants-simulation");
}