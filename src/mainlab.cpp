#include "engine/world.h"
#include "engine/ant.h"
#include "NEAT/population.h"
#include "NEAT/ComputeFitness.h"
#include <fstream>
#include <iostream>

using namespace simu;

RNG rng;

class Scene : public WorldListener {
    std::vector<std::weak_ptr<AntIA>> ants;
    Population mPop;
    ComputeFitness compute_fitness;

    int m_count = 0;
    const int num_generations = 10; // Nombre total de générations
    const int num_ants = 10;        // Nombre de fourmis par génération
    int current_generation = 0;

    std::vector<double> avg_fitness_per_gen; // Suivi des fitness moyennes

public:
    Scene() : mPop((NeatConfig){}, rng), compute_fitness(rng) {}

    void onInit() override {
        
        getWorld().getGrid().fromImage("rsc/maze.png");

        
        ants = getWorld().spawnEntities<AntIA>(num_ants);
    }

    void onUnload() override {}

    void onDraw() override {
        if (IsKeyPressed(KEY_G)) {
            Grid &grid = getWorld().getGrid();

            for (const auto &ant : ants) {
                if (ant.expired())
                    continue;

                Vec2i antPos = grid.toTileCoord((Vec2f)(ant.lock()->getPos()));
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

        

        // Pour les calculs de fitness
        Grid &grid = getWorld().getGrid();
        Vec2i goalPos(73, 0);


        std::vector<std::shared_ptr<Genome>> genomes;
        double total_fitness = 0.0;

        for (const auto &ant : ants) {
            if (ant.expired())
                continue;

            auto locked_ant = ant.lock();
            Vec2i antPos = grid.toTileCoord((Vec2f)(locked_ant->getPos()));

            auto genome = std::make_shared<Genome>(locked_ant->getGenome());

            // Évaluation de la fitness ainsi que les actions de la fourmi vu que tout est encapsulé dans la fonction
            double fitness = compute_fitness.evaluate_lab(*genome, antPos, goalPos, grid);
            total_fitness += fitness;

            genomes.push_back(genome);
        }

        // Calcul de la fitness moyenne
        double avg_fitness = total_fitness / ants.size();
        avg_fitness_per_gen.push_back(avg_fitness);

        std::cout << "Génération " << current_generation + 1
                  << " - Fitness moyenne: " << avg_fitness << std::endl;

        // Remplacement par la nouvelle génération
        auto new_genomes = mPop.reproduce_from_genomes(genomes);
        ants.clear();

        for (auto &genome : new_genomes) {
            ants.push_back(getWorld().spawnEntity<AntIA>(*genome));//De ce que j'ai compris c'est censé être automatique la gestion d'id de fourmi non ?


        }

        current_generation++;
    }

    // Exporter la fitness moyenne
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

    // Exporter les données après la simulation
    scene->export_fitness_data();

    return result;
}
