#include "ComputeFitness.h"
#include "NeuralNetwork.h"
#include "Genome.h"
#include "../engine/ant.h"
#include <iostream>
#include <cmath> // Pour calculer la distance
#include <algorithm> // Pour std::max_element
#include "Utils.h"

using namespace simu;

// Constructeur qui initialise la référence RNG
ComputeFitness::ComputeFitness(RNG &rng) : rng(rng) {}

// Surcharge de l'opérateur () pour évaluer la fitness d'un génome
double ComputeFitness::operator()(const Genome &genome, int ant_id) const {
    return evaluate(genome, ant_id);  // Appelle la méthode d'évaluation avec l'identifiant de la fourmi
}

// Méthode pour évaluer la fitness d'une fourmi spécifique (via son ant_id)
double ComputeFitness::evaluate(const Genome &genome, int ant_id) const {
  /*  std::cout << "Evaluating genome ID: " << genome.get_genome_id() << " for ant " << ant_id << std::endl;

    // Simuler l'exécution de la fourmi dans le labyrinthe
    // Récupérer la position de la fourmi identifiée par ant_id
    double ant_x = get_ant_position_x(ant_id);  // Position actuelle de la fourmi
    double ant_y = get_ant_position_y(ant_id);
    
    double goal_x = get_goal_position_x(ant_id);  // Position de la sortie du labyrinthe (identique pour toutes les fourmis)
    double goal_y = get_goal_position_y(ant_id);

    // Calculer la distance euclidienne jusqu'à la sortie
    double distance_to_goal = std::sqrt(std::pow(goal_x - ant_x, 2) + std::pow(goal_y - ant_y, 2));

    //TODO : Créer la méthode get_maze_size pour obtenir la taille du labyrinthe
    // Normaliser la distance
    double normalized_distance = distance_to_goal / get_maze_size();

    // Calculer une fitness de base en fonction de la distance
    double fitness = 1.0 / (normalized_distance + 1);  // Plus la distance est petite, plus la fitness est élevée

    //Plus la fourmi monte en altitude, plus elle est récompensée
    double altitude = get_altitude(ant_id);
    fitness += altitude;  // Ajouter l'altitude à la fitness

    // Si la fourmi a atteint l'arrivée, lui attribuer un bonus
    if (has_reached_goal(ant_id)) {
        fitness += 100;  // Bonus si la fourmi a atteint l'objectif
    }
    return fitness;  // Retourner la fitness finale
*/
    return 0;
}

double ComputeFitness::evaluate_rpc(const Genome &genome, int ant_id) const {
    FeedForwardNeuralNetwork network = FeedForwardNeuralNetwork::create_from_genome(genome);

    int wins = 0;
    int rounds = 10;

    // Stratégie fixe de l'adversaire : alterner entre "Papier", "Ciseaux", et "Pierre"
    int opponent_moves[] = {1, 1, 1};
    

    for (int round = 0; round < rounds; ++round) {
        int opponent_move = opponent_moves[round % 3];  // Alternance fixe

        //Affichage le mouvement de l'adversaire
        //std::cout << "Opponent move: " << opponent_move << std::endl;

        // Obtenir l'action du réseau neuronal
        std::vector<double> inputs = { double(opponent_move) };
        std::vector<double> outputs = network.activate(inputs);

        // Décoder l'action
        int player_move = std::distance(outputs.begin(), std::max_element(outputs.begin(), outputs.end()));

        // Calculer le résultat
        int result = (3 + player_move - opponent_move) % 3 - 1;

        if (result == 1) wins++;
    }

    return static_cast<double>(wins) / rounds;
}

double ComputeFitness::evaluate_lab(const simu::Vec2i &startPos, const simu::Vec2i &goalPos, simu::Grid &grid, simu::AntIA &ant, double initial_distance,int current_generation) const {
    // Distance actuelle après que la fourmi ait exécuté ses actions
    int directionChanges = ant.getDirectionChanges();
    int repeatCount = ant.getRepeatCount();
    int wallHit = ant.getWallHit();
    int goodWallAvoidanceMoves = ant.getGoodWallAvoidanceMoves();
    int numberOfCheckpoints = ant.getNumberOfCheckpoints();
    
    std::unordered_set<std::pair<int, int>, simu::pair_hash> visitedPositions = ant.getVisitedPositions();
    Vec2i antPos = ant.getGridPos();


    double current_distance = static_cast<double>(grid.findPath(antPos, goalPos).size());
    double far_from_goal = static_cast<double>(grid.findPath(startPos, antPos).size());

    

    // Calcul de la fitness
   double fitness = (initial_distance - current_distance)*2;
   
    fitness += far_from_goal;
    

    
    // Ajouter une pénalité à la fitness si trop de répétitions
    if(ant.isIdle())
    {
        fitness -= 1000.0;
    }

    if(ant.isStuck())
    {
        fitness -= 1000.0;
    }
    
   
    // Critère d'exploration pour les premières générations
    if (current_generation < 100) { // Par exemple, encourager l'exploration pendant 50 générations
        fitness += visitedPositions.size() * 2.0;
        //printf("Visited positions: %d\n", visitedPositions.size());
    if (directionChanges < 3) {
        fitness -= 50.0;  // Pénalité modérée
    } else if (directionChanges >= 5 && directionChanges <= 10) {
        fitness += 30.0;  // Récompense pour exploration modérée
    } else {
        fitness -= 20.0;  // Légère pénalité pour trop de changements
    }
    fitness += visitedPositions.size() * 1.0;
}


    // Pénaliser les collisions avec les murs
    fitness -= wallHit * 10.0;

    fitness += goodWallAvoidanceMoves * 5.0;



    if(ant.isEnd())
    {
        fitness += 12000.0;
    }

    fitness += numberOfCheckpoints * 1000.0;

    // Ajouter une récompense supplémentaire si la fourmi atteint l'objectif
    if (antPos == goalPos) {
        fitness += 12000.0;
    }

    
    
    
    // Retourner la fitness finale, en s'assurant qu'elle n'est pas négative
    return fitness ;
}



