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
            LaborerIA(const long id, std::vector<Vec2i>* foodPos, Vec2i spawnPos) : Ant(id, getWorld().gridToWorld(spawnPos)), m_genome(Genome::create_minimal_genome(4, 2, gRng))
                                                                                ,m_network(FeedForwardNeuralNetwork::create_from_genome(m_genome)), m_spawnPos(spawnPos), m_foodPos(foodPos) {};

            LaborerIA(const long id, std::vector<Vec2i>* foodPos, const Genome genome, Vec2i spawnPos) : Ant(id, getWorld().gridToWorld(spawnPos)), m_genome(genome)
                                                                                                    ,m_network(FeedForwardNeuralNetwork::create_from_genome(m_genome)), m_spawnPos(spawnPos), m_foodPos(foodPos) {};
            
            
            const char* getType() const override { return "laborerIA"; };
            const Genome& getGenome() { return m_genome; };
            const FeedForwardNeuralNetwork& getNetwork() const { return m_network; };

            void update() override
            {
                // Vec2f spawnPos = getWorld().gridToWorld(m_spawnPos);
                Vec2i gridPos = getWorld().getGrid().toTileCoord(m_pos);
              //  Vec2i nearestFood = getNearestFood(gridPos);

                Vec2i nearestFood(86, 86);
                
                float angle = m_velocity.angle(nearestFood - gridPos);
                float distance = nearestFood.manhattan(gridPos);
                
                std::vector<double> inputs = {m_angle, angle, distance, 1.0};
                auto outputs = m_network.activate(inputs);

                int action = std::distance(outputs.begin(), std::max_element(outputs.begin(), outputs.end()));
                
                std::random_device rd;  // Générateur aléatoire
                std::mt19937 gen(rd()); // Mersenne Twister
                std::uniform_real_distribution<> dis(-0.1, 0.1); // Bruit entre -0.1 et 0.1
                for(int i = 0; i < outputs.size(); i++)
                {
                    outputs[i] += dis(gen);
                }
                
                
                switch(action)
                {
                    case 0:
                        rotate(m_angle += 0.1);
                        break;
                    case 1:
                        rotate(m_angle -= 0.1);
                        break;
                    case 2:
                        break;
                }
                moveForward();

                if(getTileOn().type == Type::FOOD)
                {
                    eat();
                    m_harvestedFood++;
                }
            };

            // TODO: Effectuer un binary search si la liste est triée
            Vec2i getNearestFood( Vec2i pos)
            {
                Vec2i nearestFood = Vec2i(0, 0);
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
                    {
                        nearestFood = foodPos;
                        minDist = dist;
                    }
                }
                return nearestFood;
            };

            float getAngle() const { return m_angle; };

            int getHarvestedFood() const { return m_harvestedFood; };

        private:
            Genome m_genome;
            FeedForwardNeuralNetwork m_network;
            Vec2i m_spawnPos;
            std::vector<Vec2i>* m_foodPos; // Solution pourris
            int m_harvestedFood = 0;
    };

    class Laborer: public Level
    {
        static constexpr int m_popSize = 100;
        static constexpr int m_nbrTickDay = 500;
        const Vec2i m_spawnPos = Vec2i(80, 80);
        
        int m_tickCount = 0;
        int m_generation = 0;

        Population m_pop;
        std::vector<std::weak_ptr<LaborerIA>> m_laborers;

        std::vector<Vec2i> m_foodPos;

        public:
            Laborer(std::string name): Level(name), m_pop((NeatConfig) { .population_size = m_popSize, .num_inputs = 4, .num_outputs = 2}, gRng) {};
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
                m_generation++;

                std::vector<std::shared_ptr<Genome>> genomes;
                std::vector<double> fitnesses;

                for(const auto& laborer: m_laborers)
                {
                    if(laborer.expired())
                        continue;

                    fitnesses.push_back(calculFitness(laborer));
                    genomes.push_back(std::make_shared<Genome>(laborer.lock()->getGenome()));
                }
                
                auto new_genomes = m_pop.reproduce_from_genomes_with_fitness(genomes, fitnesses);
                m_laborers.clear();
                getWorld().clearEntities();

                for (auto &genome : new_genomes) 
                {
                    m_laborers.push_back(getWorld().spawnEntity<LaborerIA>(&m_foodPos, *genome.genome, m_spawnPos));
                }

               generateFood();

               TraceLog(LOG_INFO, "Génération terminée n°%d", m_generation);
            }

            double calculFitness(const std::weak_ptr<LaborerIA> ia)
            {
                if(ia.expired())
                    return 0.0;

                auto laborer = ia.lock();
                Vec2i pos = getWorld().getGrid().toTileCoord(laborer->getPos());

                float dist = laborer->getNearestFood(pos).manhattan(pos);
                dist = 100.f / dist; 

                return laborer->getHarvestedFood() * 100 + dist;
            }

            // TODO: Trier les positions pour accélérer la recherche (avec binary_search)
            void generateFood()
            {
                m_foodPos.clear();

                m_foodPos.push_back(Vec2i(86, 86));
                getWorld().getGrid().setTile(FOOD, 86, 86);
                /*Grid& grid = getWorld().getGrid();
                grid.init(160);

                for(int i = 0; i < 100; i++)
                {
                    Vec2i pos = Vec2i(gRng.uniform(0, grid.getGridWidth()), gRng.uniform(0, grid.getGridWidth()));
                    grid.setTile(FOOD, pos.x, pos.y);
                    m_foodPos.push_back(pos);
                }*/
            }
    };
}

#endif // LABORER_H