#ifndef LABORER_H
#define LABORER_H

#include "../engine/world.h"
#include "../engine/types.h"
#include "../NEAT/Genome.h"
#include "../NEAT/NeuralNetwork.h"
#include "../NEAT/population.h"
#include <deque>
#include <set>

namespace simu
{
    RNG gRng;

    class LaborerIA : public Ant
    {
    public:
        LaborerIA(const long id, std::vector<Vec2i> *foodPos, Vec2i spawnPos)
            : Ant(id, getWorld().gridToWorld(spawnPos)),
              m_genome(Genome::create_minimal_genome(8, 1, gRng)),
              m_network(FeedForwardNeuralNetwork::create_from_genome(m_genome)),
              m_spawnPos(spawnPos),
              m_foodPos(foodPos) {}

        LaborerIA(const long id, std::vector<Vec2i> *foodPos, Genome genome, Vec2i spawnPos)
            : Ant(id, getWorld().gridToWorld(spawnPos)),
              m_genome(genome),
              m_network(FeedForwardNeuralNetwork::create_from_genome(m_genome)),
              m_spawnPos(spawnPos),
              m_foodPos(foodPos) {}

        const char *getType() const override { return "laborerIA"; }
        const Genome &getGenome() { return m_genome; }
        const FeedForwardNeuralNetwork &getNetwork() const { return m_network; }

        void update() override
        {
            Vec2i gridPos = getWorld().getGrid().toTileCoord(m_pos);
            Vec2i nearestFood = getNearestFood(gridPos);

            float angle = m_velocity.angle(nearestFood - gridPos);
            float distance = nearestFood.manhattan(gridPos);

            bool is_stuck = isInCycle(gridPos);

            double velocityLength = sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);

            std::vector<double> inputs = {
                m_angle, angle, distance, velocityLength,
                static_cast<double>(getTileFacing().flags.solid),
                static_cast<double>(getTileLeft().flags.solid),
                static_cast<double>(getTileRight().flags.solid),
                static_cast<double>(is_stuck)};

            auto outputs = m_network.activate(inputs);
            int action = std::distance(outputs.begin(), std::max_element(outputs.begin(), outputs.end()));

            moveForward();

            double max_rotation_speed = 0.1;
            double rotation = (outputs[0] - 0.5) * 0.05;
            if (gRng.uniform(0.0, 1.0) < 0.02)
            {
                m_angle += gRng.uniform(-0.3, 0.3);
            }

            rotate(m_angle += rotation * max_rotation_speed);

            if (getTileOn().type == Type::FOOD)
            {
                eat();
                m_harvestedFood++;
            }

            trackPosition(gridPos);
        }

        bool isInCycle(const Vec2i &pos)
        {
            return std::count(recent_positions.begin(), recent_positions.end(), pos) > 2;
        }

        void trackPosition(const Vec2i &pos)
        {
            recent_positions.push_back(pos);
            if (recent_positions.size() > max_recent_positions)
            {
                recent_positions.pop_front();
            }
        }

        Vec2i getNearestFood(const Vec2i &pos)
        {
            Vec2i nearestFood;
            float minDist = std::numeric_limits<float>::max();
            for (size_t i = 0; i < m_foodPos->size(); i++)
            {
                if (getWorld().getGrid().getTile(m_foodPos->at(i)).type != Type::FOOD)
                {
                    m_foodPos->erase(m_foodPos->begin() + i);
                    i--;
                    continue;
                }

                Vec2i foodPos = m_foodPos->at(i);
                float dist = foodPos.manhattan(pos);
                if (dist < minDist)
                {
                    nearestFood = foodPos;
                    minDist = dist;
                }
            }
            return nearestFood;
        }

        int getHarvestedFood() const { return m_harvestedFood; }
        int getUniqueVisitedPositions() const { return unique_positions.size(); }

    private:
        Genome m_genome;
        FeedForwardNeuralNetwork m_network;
        Vec2i m_spawnPos;
        std::vector<Vec2i> *m_foodPos;
        int m_harvestedFood = 0;
        float m_angle = 0;

        std::deque<Vec2i> recent_positions;
        const int max_recent_positions = 10;
        std::set<Vec2i> unique_positions;
    };

    class Laborer : public Level
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
        Laborer(std::string name)
            : Level(name),
              m_pop((NeatConfig){.population_size = m_popSize, .num_inputs = 8, .num_outputs = 1}, gRng) {}

        const std::string getDescription() const override { return "Apprentissage de récolte de nourriture."; }

        void onInit() override
        {
            getWorld().getGrid().init(160);
            generateFood();
            m_laborers = getWorld().spawnEntities<LaborerIA>(m_popSize, &m_foodPos, m_spawnPos);
        }

        void onUpdate() override
        {
            if (++m_tickCount < m_nbrTickDay)
                return;

            m_tickCount = 0;
            m_generation++;

            std::vector<std::shared_ptr<Genome>> genomes;
            std::vector<double> fitnesses;

            for (const auto &laborer : m_laborers)
            {
                if (laborer.expired())
                    continue;

                fitnesses.push_back(calculFitness(laborer));
                genomes.push_back(std::make_shared<Genome>(laborer.lock()->getGenome()));
            }

            auto new_genomes = m_pop.reproduce_from_genome_roulette(genomes, fitnesses);
            m_laborers.clear();
            getWorld().clearEntities();

            for (auto &genome : new_genomes)
            {
                m_laborers.push_back(getWorld().spawnEntity<LaborerIA>(&m_foodPos, *genome.genome.get(), m_spawnPos));
            }

            generateFood();
            TraceLog(LOG_INFO, "Génération terminée n°%d", m_generation);
        }

        double calculFitness(const std::weak_ptr<LaborerIA> ia)
        {
            if (ia.expired())
                return 0.0;

            auto laborer = ia.lock();
            Vec2i pos = getWorld().getGrid().toTileCoord(laborer->getPos());

            float dist = laborer->getNearestFood(pos).manhattan(pos);
            double exploration_bonus = laborer->getUniqueVisitedPositions() * 10;
            double penalty_cycle = laborer->isInCycle(pos) ? -200 : 0;

            return laborer->getHarvestedFood() * 100 + (1.0 / (1.0 + dist)) * 50 + exploration_bonus + penalty_cycle;
        }

        void generateFood()
        {
            m_foodPos.clear();
            Grid &grid = getWorld().getGrid();

            for (int i = 0; i < 10; i++)
            {
                Vec2i pos;
                do
                {
                    pos = Vec2i(gRng.uniform(0, grid.getGridWidth()), gRng.uniform(0, grid.getGridWidth()));
                } while (grid.getTile(pos).flags.solid);

                grid.setTile(FOOD, pos.x, pos.y);
                m_foodPos.push_back(pos);
            }
        }
    };
}

#endif // LABORER_H
