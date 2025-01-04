    #ifndef COMPUTEFITNESS_H
    #define COMPUTEFITNESS_H

    #include "RNG.h"  // Inclure la classe RNG (générateur de nombres aléatoires)
    #include "Genome.h"  // Inclure la définition du Genome
    #include "../engine/tiles.h"  
    #include "../engine/types.h"

    class ComputeFitness {
    public:
        // Constructeur qui prend un générateur RNG en référence
        ComputeFitness(RNG &rng);

        // Surcharge de l'opérateur () pour évaluer la fitness d'un génome
        double operator()(const Genome &genome,int ant_id) const;

        // Méthode pour évaluer la fitness d'un génome (si besoin d'une version nommée)
        double evaluate(const Genome &genome, int ant_id) const;

        double evaluate_rpc(const Genome &genome, int ant_id) const;

        double evaluate_lab(const Genome &genome, const simu::Vec2i &startPos, const simu::Vec2i &goalPos, simu::Grid &grid) const;
        

    private:
        RNG &rng;  // Référence au générateur RNG utilisé pour l'évaluation
        
    };

    #endif // COMPUTEFITNESS_H
