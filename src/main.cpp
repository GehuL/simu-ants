#include "engine/world.h"
#include "engine/ant.h"
#include "NEAT/population.h"

using namespace simu;

RNG rng;

class Scene: public WorldListener
{
    std::vector<std::weak_ptr<DemoAnt>> ants;

    public:
        Scene(): mPop((NeatConfig) {}, rng) {};

        void onInit() override
        {
            getWorld().getGrid().fromImage("rsc/maze.png");
            m_ants = getWorld().spawnEntities<AntIA>(10);
        };

            double deltaTime = (GetTime() - startTime) * 1000.f;
            TraceLog(LOG_DEBUG, "A*: %.1lf ms", deltaTime);
        
        void onUpdate() override
        {
            if(m_count ++  < 1000)
                return;
            
            m_count = 0;

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

            for(auto genome: newlies)
            {
                
                m_ants.push_back(getWorld().spawnEntity<AntIA>(*genome.genome.get()));
            }

            ants = getWorld().spawnEntities<DemoAnt>(10, getWorld().gridToWorld(Vec2i{89, 161}));
        };

    private:
        int m_count = 0;
        std::vector<std::weak_ptr<AntIA>> m_ants;
        Population mPop;
};

int main(void)
{   
    SetTraceLogLevel(LOG_DEBUG);

    simu::World& world = simu::getWorld(); 
    world.setListener(std::make_shared<Scene>());
    return world.run(800, 800, "ants-simulation");
}