#include "engine/world.h"
#include "engine/ant.h"
#include "NEAT/population.h"
#include "raylib.h"

#include "external/json.hpp"

using json = nlohmann::json;
using namespace simu;

RNG rng;

class Scene: public WorldListener
{
    std::vector<std::weak_ptr<AntIA>> m_ants;
    
    const Vec2i spawnPosition;
    const std::string genomeFile = "genomes.json";

    public:
        Scene(): mPop((NeatConfig) {}, rng) {};

        void onInit() override
        {
            getWorld().getGrid().fromImage("rsc/maze.png");
            
            loadGenomes();                
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

            m_ants.clear();
            auto newlies = mPop.reproduce_from_genomes(genomes);

            for(auto genome: newlies)
            {
                
                m_ants.push_back(getWorld().spawnEntity<AntIA>(*genome.genome.get()));
            }
            
            saveGenomes();
        };

        void saveGenomes()
        {
            std::cout << "Saving genomes to file " << genomeFile << std::endl;
            try
            {
                json j;
                for(auto& ant: m_ants)
                {
                    if(!ant.expired())
                    {
                        auto genome = ant.lock()->getGenome();
                        j["genomes"] += genome;
                    }
                }

                auto file = std::ofstream(genomeFile, std::ios_base::out);
                file << j;
                file.close();
                std::cout << "Saving successful" << std::endl;
            }catch(const json::exception& e)
            {
                std::cerr << "Error saving file " << genomeFile << ": " << e.what() << std::endl;
            }
        }

        void loadGenomes()
        {
            std::cout << "Loading genomes from " << genomeFile << std::endl;
            try
            {
                auto file = std::ifstream(genomeFile, std::ios_base::in);
                json j = json::parse(file);
                for(auto& genome: j["genomes"])
                {
                    Genome g = genome.get<Genome>();
                    auto ia = getWorld().spawnEntity<AntIA>(g);
                    ia.lock()->setPos(spawnPosition);
                    m_ants.push_back(ia);
                }
                file.close();
                std::cout << "Loading successful" << std::endl;
            }catch(const json::exception& e)
            {
                std::cerr << "Error loading file " << genomeFile << ": " << e.what() << std::endl;
                std::cout << "Spawning new ants instead" << std::endl;
                m_ants = getWorld().spawnEntities<AntIA>(10, spawnPosition);
            }
        }

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