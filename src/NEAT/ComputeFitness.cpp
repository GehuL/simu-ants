#include "ComputeFitness.h"
#include "NeuralNetwork.h"
#include "Genome.h"
#include <iostream>
#include <cmath> // Pour calculer la distance
#include <algorithm> // Pour std::max_element


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

double ComputeFitness::evaluate_lab(const Genome &genome, const Vec2i &startPos, const Vec2i &goalPos, Grid &grid) const {
    FeedForwardNeuralNetwork network = FeedForwardNeuralNetwork::create_from_genome(genome);

    Vec2i antPos = startPos;
    double initial_distance = static_cast<double>(grid.findPath(startPos, goalPos).size()); // Distance initiale avec A*
    double fitness = 0.0;

    const int max_steps = 100;  // Limite de mouvements qu'on pourra remplacer par autre chose 
    for (int step = 0; step < max_steps; ++step) {
        // Obtenir l'état actuel du jeu
        auto inputs = get_game_state_lab(antPos, goalPos, grid);

        // Calculer la sortie du réseau neuronal
        auto outputs = network.activate(inputs);

        // Effectuer l'action
        Ant ant(antPos);  // Créer une fourmi temporaire pour tester les mouvements,genre je pense que c'est comme ça que font les gens qui teste des ia sur des jeux au loieu d'avoir plusieurs objets en déplacements ?
        perform_action_lab(outputs, ant);

        // Mettre à jour la position
        antPos = ant.getPosition();

        // Calculer la distance actuelle (via A*)
        double current_distance = static_cast<double>(grid.findPath(antPos, goalPos).size());

        // Score basé sur la réduction de la distance (Comme ce que tu voulais)
        fitness += (initial_distance - current_distance);

        // Terminer si la fourmi atteint la sortie
        if (antPos == goalPos) {
            fitness += 10000.0;  // Bonus pour atteindre la sortie (Comme Sergey)
            break;
        }
    }

    // Pénalité pour les mouvements inutiles
    fitness -= 0.1 * max_steps;

    return fitness > 0 ? fitness : 0.0;  // Éviter les valeurs négatives
}

