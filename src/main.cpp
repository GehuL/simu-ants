#include "engine/world.h"
#include "engine/ant.h"
#include "NEAT/population.h"
#include "raylib.h"

using namespace simu;

RNG rng;

class Scene: public WorldListener
{
    std::vector<std::weak_ptr<AntIA>> m_ants;

    public:
        Scene(): mPop((NeatConfig) {}, rng) {};

        void onInit() override
        {
            getWorld().getGrid().fromImage("rsc/maze.png");
            m_ants = getWorld().spawnEntities<AntIA>(10, Vec2i(90, 160));
        };
        
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
<<<<<<< HEAD
=======

            if(IsKeyPressed(KEY_SPACE))
            {
                // getWorld().clearEntities();
                 for(auto& en : getWorld().getEntities())
                {
                    getWorld().removeEntity(en.lock()->getId());
                }
            }

        }
>>>>>>> ebba413 (ajout fonction supprimer entitié)

            m_ants.clear();
            auto newlies = mPop.reproduce_from_genomes(genomes);

            for(auto genome: newlies)
            {
                
                m_ants.push_back(getWorld().spawnEntity<AntIA>(*genome.genome.get()));
            }

        };

        void onDraw() override {};
        void onUnload() override {};

    private:
        int m_count = 0;
        Population mPop;
};

int main(void)
{   
    SetTraceLogLevel(LOG_DEBUG);

    simu::World& world = simu::getWorld(); 
    world.setListener(std::make_shared<Scene>());
    return world.run(800, 800, "ants-simulation");
}