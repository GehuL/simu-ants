#include "engine/world.h"
#include "engine/ant.h"
#include "NEAT/population.h"
#include "NEAT/ComputeFitness.h"
#include "NEAT/Utils.h"
#include <fstream>
#include <iostream>


using namespace simu;

RNG rng;

class Scene : public WorldListener {
    std::vector<std::weak_ptr<AntIA>> ants;
    Population mPop;
    ComputeFitness compute_fitness;

    int m_count = 0;
    const int num_generations = 1000; // Nombre total de générations
    const int num_ants = 100;        // Nombre de fourmis par génération
    int current_generation = 0;

    std::vector<double> avg_fitness_per_gen; // Suivi des fitness moyennes
    std::vector<int> steps_count;           // Compteur d'étapes par fourmi
    int max_steps;                          // Nombre maximal d'actions
    double initial_distance; // Distance initiale pré-calculée
    int current_tick = 0;                   
    int max_allowed_ticks = 0; 


public:
    Scene() : mPop((NeatConfig){}, rng), compute_fitness(rng) {}

    void onInit() override {
        getWorld().getGrid().fromImage("rsc/mazeCheck.png");
        Vec2i startPos(90, 150);
        Vec2i goalPos(73, 0);
        ants = getWorld().spawnEntities<AntIA>(num_ants, startPos);
        
        steps_count.resize(num_ants, 0); // Initialiser les compteurs d'étapes

        // Calculer le nombre maximal d'actions
        Grid &grid = getWorld().getGrid();
        
        auto path = grid.findPath(startPos, goalPos);
        double path_length = static_cast<double>(path.size());
        max_steps = static_cast<int>(path_length * 1.5);

        
        current_tick = 0;
        max_allowed_ticks = max_steps;


        initial_distance = path_length;
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

    if (current_tick >= max_allowed_ticks) {
        // Passer à la génération suivante
        finalizeGeneration();
        return;
    }

    current_tick++;
}

    void finalizeGeneration() {
    
    double total_fitness = 0.0;
    for (const auto &ant : ants) {
        if (ant.expired()) continue;

        auto locked_ant = ant.lock();
        Vec2i antPos = getWorld().getGrid().toTileCoord((Vec2f)(locked_ant->getPos()));
        // Calculer la fitness individuelle
        double fitness = compute_fitness.evaluate_lab(
            antPos, Vec2i(73, 0), getWorld().getGrid(), *locked_ant, initial_distance, current_generation
        );

        total_fitness += fitness;

        // Attribuer la fitness à l'AntIA
        locked_ant->setFitness(fitness);
    }

    double avg_fitness = total_fitness / ants.size();
    avg_fitness_per_gen.push_back(avg_fitness);

    std::cout << "Génération " << current_generation + 1
              << " - Fitness moyenne: " << avg_fitness << std::endl;

    // Réinitialiser pour la prochaine génération
    nextGeneration();
}

    void nextGeneration() {
    std::vector<std::shared_ptr<Genome>> genomes;
    std::vector<double> fitnesses;
    for (const auto &ant : ants) {
        if (!ant.expired()) {
            genomes.push_back(std::make_shared<Genome>(ant.lock()->getGenome()));
            fitnesses.push_back(ant.lock()->getFitness());
        }
    }

    auto new_genomes = mPop.reproduce_from_genome_roulette_negative(genomes, fitnesses);
    ants.clear();

    getWorld().clearEntities();

    for (auto &genome : new_genomes) {
        ants.push_back(getWorld().spawnEntity<AntIA>(*genome.genome, Vec2i(90, 150)));
    }

    // Réinitialiser le compteur global et les positions
    current_tick = 0;
    

    current_generation++;
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
