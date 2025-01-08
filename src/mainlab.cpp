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
    const int num_generations = 100; // Nombre total de générations
    const int num_ants = 10;        // Nombre de fourmis par génération
    int current_generation = 0;

    std::vector<double> avg_fitness_per_gen; // Suivi des fitness moyennes
    std::vector<int> steps_count;           // Compteur d'étapes par fourmi
    int max_steps;                          // Nombre maximal d'actions
    double initial_distance; // Distance initiale pré-calculée


public:
    Scene() : mPop((NeatConfig){}, rng), compute_fitness(rng) {}

    void onInit() override {
        getWorld().getGrid().fromImage("rsc/maze.png");
        Vec2i startPos(90, 160);
    Vec2i goalPos(73, 0);
        ants = getWorld().spawnEntities<AntIA>(num_ants);
        for (const auto &ant : ants) {
            if (ant.expired())
                continue;

            auto locked_ant = ant.lock();
            locked_ant->setPos(getWorld().gridToWorld(startPos));
        }
        
        steps_count.resize(num_ants, 0); // Initialiser les compteurs d'étapes

        // Calculer le nombre maximal d'actions
        Grid &grid = getWorld().getGrid();
        
        auto path = grid.findPath(startPos, goalPos);
        double path_length = static_cast<double>(path.size());
        max_steps = static_cast<int>(path_length * 1.5);

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

        Grid &grid = getWorld().getGrid();

        bool all_done = true;
        double total_fitness = 0.0;
        Vec2i startPos(90, 160);
        Vec2i goalPos(73, 0);

        for (size_t i = 0; i < ants.size(); ++i) {
            auto &ant = ants[i];
            if (ant.expired())
                continue;

            auto locked_ant = ant.lock();
            Vec2i antPos = grid.toTileCoord((Vec2f)(locked_ant->getPos()));

            // Vérifier si la fourmi peut encore effectuer des actions
            if (steps_count[i] < max_steps) {
                /*
                auto inputs = get_game_state_lab(antPos,  grid);
                std::cout << "Activation du réseau de neurones avec " << inputs.size() << " entrées." << std::endl;
                auto outputs = const_cast<FeedForwardNeuralNetwork&>(locked_ant->getNetwork()).activate(inputs);

                perform_action_lab(outputs, *locked_ant);

                 // Ajout des logs pour suivre les actions des fourmis
                int direction = std::distance(outputs.begin(), std::max_element(outputs.begin(), outputs.end()));
                std::cout << "Fourmi " << i << " action: " << direction << std::endl;
*/
                steps_count[i]++;
                all_done = false; // Au moins une fourmi n'a pas fini


            }

            // Évaluer la fitness après les étapes
            if (steps_count[i] == max_steps) {
                double fitness = compute_fitness.evaluate_lab(antPos, goalPos, grid, *locked_ant, initial_distance);

                total_fitness += fitness;
            }
        }

        if (all_done) {
            // Calcul de la fitness moyenne
            double avg_fitness = total_fitness / ants.size();
            avg_fitness_per_gen.push_back(avg_fitness);

            std::cout << "Génération " << current_generation + 1
                      << " - Fitness moyenne: " << avg_fitness << std::endl;

            // Remplacement par la nouvelle génération
            std::vector<std::shared_ptr<Genome>> genomes;
            for (const auto &ant : ants) {
                if (!ant.expired()) {
                    genomes.push_back(std::make_shared<Genome>(ant.lock()->getGenome()));
                }
            }

            auto new_genomes = mPop.reproduce_from_genomes(genomes);
            ants.clear();
            steps_count.clear();

            for (auto &genome : new_genomes) {
                ants.push_back(getWorld().spawnEntity<AntIA>(*genome.genome));
                for (const auto &ant : ants) {
            if (ant.expired())
                continue;

            auto locked_ant = ant.lock();
            locked_ant->setPos(getWorld().gridToWorld(startPos));
        }
                steps_count.push_back(0); // Réinitialiser les compteurs d'étapes

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
