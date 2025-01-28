#ifndef LABORER_H
#define LABORER_H

#include "../engine/world.h"
#include "../engine/types.h"
#include "../NEAT/Genome.h"
#include "../NEAT/NeuralNetwork.h"
#include "../NEAT/population.h"

namespace simu
{
    RNG gRng;

    class LaborerIA: public Ant
    {

        public:
            LaborerIA(const long id, std::vector<Vec2i>* foodPos, Vec2i spawnPos) : Ant(id, getWorld().gridToWorld(spawnPos)), m_genome(Genome::create_diverse_genome(0, 6, 1, 1, gRng))
                                                                                ,m_network(FeedForwardNeuralNetwork::create_from_genome(m_genome)), m_spawnPos(spawnPos), m_foodPos(foodPos) {};

            LaborerIA(const long id, std::vector<Vec2i>* foodPos, const Genome& genome, Vec2i spawnPos) : Ant(id, getWorld().gridToWorld(spawnPos)), m_genome(genome)
                                                                                                    ,m_network(FeedForwardNeuralNetwork::create_from_genome(m_genome)), m_spawnPos(spawnPos), m_foodPos(foodPos) {};
            
            
            const char* getType() const override { return "laborerIA"; };
            const Genome& getGenome() const { return m_genome; };
            const FeedForwardNeuralNetwork& getNetwork() const { return m_network; };

            void update() override
            {
                Vec2f spawnPos = getWorld().gridToWorld(m_spawnPos);
                float nearestFood = getNearestFood(getWorld().getGrid().toTileCoord(m_pos));

                std::vector<double> inputs = {m_angle, m_pos.x, m_pos.y, spawnPos.x, spawnPos.y, nearestFood};
                
                auto outputs = m_network.activate(inputs);
               
              /*int direction = std::distance(outputs.begin(), std::max_element(outputs.begin(), outputs.end()));
                switch(direction)
                {
                    case 0: m_angle = EAST; break;
                    case 1: m_angle = NORTH; break;
                    case 2: m_angle = WEST; break;
                    case 3: m_angle = SOUTH; break;
                }*/

                m_angle = outputs[0] * 2 * PI - PI;
                move(m_angle);

                if(getTileOn().type == Type::FOOD)
                {
                    eat();
                    m_harvestedFood++;
                }
            };

            // TODO: Effectuer un binary search si la liste est triée
            float getNearestFood( Vec2i pos)
            {
                float minDist = std::numeric_limits<float>::max();
                for(size_t i = 0; i < m_foodPos->size(); i++)
                {
                    // Vérifier si la nourriture existe toujours
                    if(getWorld().getGrid().getTile(m_foodPos->at(i)).type != Type::FOOD) {
                        m_foodPos->erase(m_foodPos->begin() + i);
                        i--;
                        continue;
                    }
                    
                    Vec2i foodPos = m_foodPos->at(i);
                    float dist = foodPos.manhattan(pos);
                    if(dist < minDist)
                        minDist = dist;
                }
                return minDist;
            };

            int getHarvestedFood() const { return m_harvestedFood; };

        private:
            Genome m_genome;
            FeedForwardNeuralNetwork m_network;
            Vec2i m_spawnPos;
            std::vector<Vec2i>* m_foodPos; // Solution pourris
            int m_harvestedFood = 0;
            float m_angle = 0;
    };

    class Laborer: public Level
    {
        static constexpr int m_popSize = 50;
        const int m_nbrTickDay = 5000;
        const Vec2i m_spawnPos = Vec2i(80, 80);
        
        int m_tickCount = 0;
        Population m_pop;
        std::vector<std::weak_ptr<LaborerIA>> m_laborers;

        std::vector<Vec2i> m_foodPos;

        public:
            Laborer(std::string name): Level(name), m_pop((NeatConfig) { .population_size = 50, .num_inputs = 6, .num_outputs = 4}, gRng) {};
            const std::string getDescription() const override { return "Apprentissage de récolte de nourriture."; };

            void onInit() override
            {
                Grid& grid = getWorld().getGrid();
                grid.init(160);

                generateFood();
                m_laborers = getWorld().spawnEntities<LaborerIA>(m_popSize, &m_foodPos, m_spawnPos);
            }

            void onUpdate() override
            {
                if(++m_tickCount < m_nbrTickDay)
                    return;

                m_tickCount = 0;

                std::vector<std::shared_ptr<Genome>> genomes(m_laborers.size());
                std::vector<double> fitnesses(m_laborers.size());

                for(const auto& laborer: m_laborers)
                {
                    if(laborer.expired())
                        continue;

                    fitnesses.push_back(calculFitness(laborer));
                    genomes.push_back(std::make_shared<Genome>(laborer.lock()->getGenome()));
                }

                auto new_genomes = m_pop.reproduce_from_genome_roulette(genomes, fitnesses);
                getWorld().clearEntities();
                m_laborers.clear();

                for (auto &genome : new_genomes) 
                    m_laborers.push_back(getWorld().spawnEntity<LaborerIA>(&m_foodPos, *genome.genome, m_spawnPos));

               generateFood();
            }

            double calculFitness(const std::weak_ptr<LaborerIA> ia)
            {
                if(ia.expired())
                    return 0.0;

                auto laborer = ia.lock();
                return laborer->getHarvestedFood() * 10;
            }

            // TODO: Trier les positions pour accélérer la recherche (avec binary_search)
            void generateFood()
            {
                m_foodPos.clear();

                Grid& grid = getWorld().getGrid();
                grid.init(160);

                for(int i = 0; i < 100; i++)
                {
                    Vec2i pos = Vec2i(gRng.uniform(0, grid.getGridWidth()), gRng.uniform(0, grid.getGridWidth()));
                    grid.setTile(FOOD, pos.x, pos.y);
                    m_foodPos.push_back(pos);
                }
            }
    };
}

#endif // LABORER_H