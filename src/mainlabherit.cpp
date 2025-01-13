#include "engine/world.h"
#include "engine/ant.h"
#include "NEAT/population.h"
#include "NEAT/ComputeFitness.h"
#include <fstream>
#include <iostream>

using namespace simu;

RNG rng;

class Scene : public WorldListener {
    std::vector<std::shared_ptr<AntIALab>> ants;
    Population mPop;
    ComputeFitness compute_fitness;

    int m_count = 0;
    const int num_generations = 100; // Nombre total de générations
    const int num_ants = 10;        // Nombre de fourmis par génération
    int current_generation = 0;

    std::vector<double> avg_fitness_per_gen; // Suivi des fitness moyennes

public:
    Scene() : mPop((NeatConfig){}, rng), compute_fitness(rng) {}

    void onInit() override {
        getWorld().getGrid().fromImage("rsc/maze.png");
        Grid &grid = getWorld().getGrid();

        Vec2i goalPos(73, 0);
        Vec2i startPos(90, 160);
        ants.clear();

        for (int i = 0; i < num_ants; ++i) {
            auto path = grid.findPath(startPos, goalPos);
            int max_steps = static_cast<int>(path.size() * 1.5);
            auto ant = getWorld().spawnEntity<AntIALab>(ants);
            ants.push_back(ant);
        }
    }

    void onUnload() override {}

    void onDraw() override {
        if (IsKeyPressed(KEY_G)) {
            Grid &grid = getWorld().getGrid();

            for (const auto &ant : ants) {
                if (!ant)
                    continue;

                Vec2i antPos = grid.toTileCoord((Vec2f)(ant->getPos()));
                auto path = grid.findPath(antPos, getWorld().mouseToGridCoord());

                const int tileSize = grid.getTileSize();
                for (Vec2i tile : path) {
                    DrawRectangle(tile.x * tileSize, tile.y * tileSize, tileSize, tileSize, RED);
                }
            }
        }
    }

    void onUpdate() override {
        if (current_generation >= num_generations) {
            std::cout << "Simulation terminée." << std::endl;
            return;
        }

        Grid &grid = getWorld().getGrid();
        Vec2i goalPos(73, 0);

        bool all_done = true;
        double total_fitness = 0.0;

        for (auto &ant : ants) {
            if (!ant || !ant->canAct()) 
                continue;

            ant->act(grid);
            all_done = false; // Au moins une fourmi n'a pas fini
        }

        if (all_done) {
            // Calcul de la fitness moyenne
            for (auto &ant : ants) {
                double fitness = compute_fitness.evaluate_lab(
                    grid.toTileCoord(ant->getPos()), goalPos, grid, *ant);
                total_fitness += fitness;
            }

            double avg_fitness = total_fitness / ants.size();
            avg_fitness_per_gen.push_back(avg_fitness);

            std::cout << "Génération " << current_generation + 1
                      << " - Fitness moyenne: " << avg_fitness << std::endl;

            // Remplacement par la nouvelle génération
            std::vector<std::shared_ptr<Genome>> genomes;
            for (auto &ant : ants) {
                genomes.push_back(std::make_shared<Genome>(ant->getGenome()));
            }

            auto new_genomes = mPop.reproduce_from_genomes(genomes);
            ants.clear();

            for (auto &genome : new_genomes) {
                auto path = grid.findPath(Vec2i(0, 0), goalPos);
                int max_steps = static_cast<int>(path.size() * 1.5);
                ants.push_back(getWorld().spawnEntity<AntIALab>(*genome.genome, goalPos, max_steps));
            }

            current_generation++;
        }
    }

    void export_fitness_data() {
        std::ofstream file("fitness_moyenne_lab.csv");
        if (file.is_open()) {
            for (size_t i = 0; i < avg_fitness_per_gen.size(); ++i) {
                file << i + 1 << "," << avg_fitness_per_gen[i] << "\n";
            }
            file.close();
            std::cout << "Données exportées dans 'fitness_moyenne_lab.csv'.\n";
        }
    }
};

int main(void) {
    SetTraceLogLevel(LOG_DEBUG);

    simu::World &world = simu::getWorld();
    auto scene = std::make_shared<Scene>();
    world.setListener(scene);

    int result = world.run(800, 800, "Ants Labyrinth Simulation");

    scene->export_fitness_data();

    return result;
}
