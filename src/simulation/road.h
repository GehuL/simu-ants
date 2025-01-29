#ifndef __ROAD__H
#define __ROAD__H

#include "../engine/world.h"
#include "../engine/ant.h"
#include "../NEAT/population.h"
#include "../NEAT/ComputeFitness.h"
#include "../NEAT/Utils.h"
#include "../NEAT/NeatConfig.h"
#include <fstream>
#include <iostream>


namespace simu
{
    using namespace simu;
    using json = nlohmann::json;
    
NeatConfig config;
    class Road: public Level
    {
       std::vector<std::weak_ptr<AntIA>> ants;
    Population mPop;
    ComputeFitness compute_fitness;

    int m_count = 0;
    const int num_ants = 200;        // Nombre de fourmis par génération
    int current_generation = 0;

    const int exploration_generations = 50; // Nombre de générations d'exploration & à changer dans computefitness.cpp
    int max_allowed_ticks_during_explo = 0; // Ticks pour exploration
    int max_allowed_ticks = 0;            // Ticks pour générations normales
    int current_tick = 0;

    std::vector<double> avg_fitness_per_gen; // Suivi des fitness moyennes
    std::vector<double> max_fitness_per_gen; // Fitness maximum par génération
std::vector<double> min_fitness_per_gen; // Fitness minimum par génération

    std::vector<int> steps_count;           // Compteur d'étapes par fourmi
    int max_steps;                          // Nombre maximal d'actions
    double initial_distance; // Distance initiale pré-calculée

public:
    Road(const std::string& name) : Level(name), mPop((NeatConfig){}, simu::gRng), compute_fitness(simu::gRng) {}
    const std::string getDescription() const override { return "Apprentissage sur une route diagonale"; };

/*
    int generate_next_species_id() {
        static int species_id = 0;
        return species_id++;
    }
    */

    void onInit() override {
        getWorld().getGrid().fromImage("rsc/road.png");
        Vec2i startPos(90, 150);
        Vec2i goalPos(73, 0);


        ants = getWorld().spawnEntities<AntIA>(num_ants, startPos);
        
        steps_count.resize(num_ants, 0); // Initialiser les compteurs d'étapes

        // Calculer le nombre maximal d'actions
        Grid &grid = getWorld().getGrid();
        
        auto path = grid.findPath(startPos, goalPos);
        double path_length = static_cast<double>(path.size());
        max_steps = static_cast<int>(path_length * 1.5);

        max_allowed_ticks_during_explo = 26244 / 2;
        max_allowed_ticks = max_steps * 2;

        initial_distance = path_length;
        current_tick = 0;
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
        // Vérifie si le temps autorisé pour cette génération est écoulé
        int allowed_ticks = (current_generation < exploration_generations) 
                            ? max_allowed_ticks_during_explo 
                            : max_allowed_ticks;

        if (current_tick >= allowed_ticks) {
            finalizeGeneration();
            return;
        }

        current_tick++;
    }

    void speciate() {
    // Effacer les membres des espèces existantes
    mPop.clear_species();

    // Parcourir chaque fourmi et son génome
    for (const auto &ant : ants) {
        if (ant.expired()) continue;

        auto locked_ant = ant.lock();
        std::shared_ptr<Genome> genome = std::make_shared<Genome>(locked_ant->getGenome());
        bool assigned = false;

        // Comparer avec les représentants des espèces existantes
        for (auto &species : mPop.get_species_list()) {
            double distance = genome->compute_distance(species.representative, config);
            if (distance < config.compatibility_threshold) {
                species.add_member(*genome); // Ajouter le génome à l'espèce
                assigned = true;
                break;
            }
        }

        // Si aucune espèce n'est compatible, créer une nouvelle espèce
        if (!assigned) {
            mPop.create_new_species(genome);
        }
    }
}



    void finalizeGeneration() {
    double total_fitness = 0.0;
    double max_fitness = std::numeric_limits<double>::lowest();
    double min_fitness = std::numeric_limits<double>::max();

    for (const auto &ant : ants) {
        if (ant.expired()) continue;

        auto locked_ant = ant.lock();
        Vec2i antPos = getWorld().getGrid().toTileCoord((Vec2f)(locked_ant->getPos()));

        // Calculer la fitness individuelle
        double fitness = compute_fitness.evaluate_lab(
            Vec2i(90, 150), Vec2i(73, 0), getWorld().getGrid(), *locked_ant, initial_distance, current_generation
        );

        total_fitness += fitness;

        // Mettre à jour le maximum et le minimum
        if (fitness > max_fitness) {
            max_fitness = fitness;
        }
        if (fitness < min_fitness) {
            min_fitness = fitness;
        }

        // Attribuer la fitness à l'AntIA
        locked_ant->setFitness(fitness);
    }

    double avg_fitness = total_fitness / ants.size();
    avg_fitness_per_gen.push_back(avg_fitness);
    max_fitness_per_gen.push_back(max_fitness);
    min_fitness_per_gen.push_back(min_fitness);

    std::cout << "Génération " << current_generation + 1
              << " - Fitness moyenne: " << avg_fitness
              << " - Fitness max: " << max_fitness
              << " - Fitness min: " << min_fitness << std::endl;

    // Appeler la spéciation
    speciate();

    mPop.update_species_representatives();

    // Réinitialiser pour la prochaine génération
    nextGeneration();
}



    void nextGeneration() {
    std::vector<std::shared_ptr<Genome>> genomes;
    std::unordered_map<Genome, double> fitness_map;


    // Collecter les génomes et leurs fitness
    for (const auto &ant : ants) {
        if (!ant.expired()) {
            auto locked_ant = ant.lock();
            std::shared_ptr<Genome> genome = std::make_shared<Genome>(locked_ant->getGenome());
            genomes.push_back(genome);
            //std::cout << "Ajout du génome " << locked_ant->getGenome() << " avec fitness " << locked_ant->getFitness() << std::endl;
            fitness_map[locked_ant->getGenome()] = locked_ant->getFitness();

            




        }
    }

    auto species_list = mPop.get_species_list();

    // Étape 1 : Réinitialiser les espèces
    for (auto &species : species_list) {
        species.clear_members();
    }

    // Étape 2 : Réassigner les génomes aux espèces existantes
    for (const auto &genome : genomes) {
        bool assigned = false;
        for (auto &species : species_list) {
            if (species.representative.compute_distance(*genome, config) < config.compatibility_threshold) {
                species.add_member(*genome);
                assigned = true;
                break;
            }
        }

        // Si aucune espèce existante n'est compatible, créer une nouvelle espèce
        if (!assigned) {
            species_list.emplace_back(mPop.generate_next_species_id(), *genome);
        }

        //std::cout << "Taille de fitness_map: " << fitness_map.size() << std::endl;
/*
for (const auto &species : species_list) {
    std::cout << "Espèce " << species.id << " a " << species.members.size() << " membres." << std::endl;
}
*/


    }
      std::cerr << "Nombre total d'especes: " << species_list.size() << std::endl;

    // Étape 3 : Générer la nouvelle génération en fonction des espèces
    auto new_generation = mPop.reproduce_with_speciation(species_list, fitness_map);
    ants.clear();
    getWorld().clearEntities();

    for (auto &individual : new_generation) {
        ants.push_back(getWorld().spawnEntity<AntIA>(*individual.genome, Vec2i(90, 150)));
    }

    // Réinitialiser le compteur global
    current_tick = 0;
    current_generation++;
}

void export_fitness_data() {
    std::ofstream file("fitness_moyenne_lab.csv");
    if (file.is_open()) {
        file << "Generation,Fitness_Moyenne,Fitness_Max,Fitness_Min\n";
        for (size_t i = 0; i < avg_fitness_per_gen.size(); ++i) {
            file << i + 1 << ","
                 << avg_fitness_per_gen[i] << ","
                 << max_fitness_per_gen[i] << ","
                 << min_fitness_per_gen[i] << "\n";
        }
        file.close();
        std::cout << "Données exportées dans 'fitness_moyenne_lab.csv'.\n";
    }
}
    };

}

#endif 