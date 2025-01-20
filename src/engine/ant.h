#ifndef __ANT_H__
#define __ANT_H__

#include <map>
#include <string>
#include <unordered_set>

#include "types.h"
#include "entity.h"
#include "tiles.h"
#include "../NEAT/Utils.h"

#include "../NEAT/Genome.h"
#include "../NEAT/NeuralNetwork.h"

namespace simu
{

    struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

    class Ant : public Entity
    {
        public:

            Ant(const long id = -1);
            Ant(const long id, const Ant& ant);
            Ant(const long id, Vec2f position);

            virtual ~Ant() {};

            void update() override;
            void draw() override;

            void save(json& json) const override;
            void load(const json& json) override;

            bool isCarrying() const { return m_carried_object.type != Type::AIR; };
            Tile getCarriedObject() const { return m_carried_object; };

            float getLife() const { return m_life; };

            // ------ ACTIONS IA -------
            void move(Direction dir);
            void rotate(float angle);  // Définie la direction et le sens de la fourmis en radian
            bool moveForward();        // Se déplace devant elle (en fonction de son angle), renvoie vrai si aucun obstacle ne l'empeche de faire l'action
            void eat();                // Mange sur sa position (si il y a quelque chose)
            void pheromone();          // Pose un phéromone sur sa position
            void beat();               // Mord l'entité devant elle

            void take();               // Porte un objet sur elle (nourriture/mur)
            void put();                // Déposer l'objet qu'elle porte

            Ant& operator=(const Ant& en);

        private:
            float m_life = 100.0;
            Tile m_carried_object = AIR; 
    };

    /** Implémentation du comportement par défaut sans IA de la fourmis pour une démonstration.
     */
    class DemoAnt: public Ant
    {
        public:
            DemoAnt(const long id = -1);
            DemoAnt(const long id, const DemoAnt& ant);
            DemoAnt(const long id, Vector2f position);

            virtual ~DemoAnt() {};

            const char* getType() const override { return "demoAnt"; };

            void update() override;
            void save(json& json) const override;
            void load(const json& json) override;

            DemoAnt& operator=(const DemoAnt& en);

        private:
            int m_rotateCd = 0;
    };

    /**
     * Implémentation du comportement de la fourmis géré par l'IA
     */
    class AntIA: public Ant
    {
        public:
            AntIA(const long id, const AntIA& ant);
            AntIA(const long id, const Genome ant, Vec2i pos = Vec2i(0, 0));
            AntIA(const long id = -1, Vec2i position = Vec2i(0, 0));

            virtual ~AntIA() {};

            const char* getType() const override { return "antIA"; };
            const Genome& getGenome() { return m_genome; };
            const FeedForwardNeuralNetwork& getNetwork() { return m_network; };

            const Vec2i getGridPos() { return m_gridPos; };
            const int getLastAction() { return lastAction; };
            const int getDirectionChanges() { return directionChanges; };
            const int getRepeatCount() { return repeatCount; };
            const int getWallHit() { return wallHit; };
            const int getGoodWallAvoidanceMoves() { return goodWallAvoidanceMoves; };
            const int getNumberOfCheckpoints() { return numberOfCheckpoints; };
            const bool isEnd() { return end; };

            const std::unordered_set<std::pair<int, int>, pair_hash>& getVisitedPositions() { return visitedPositions; };

            bool isStuck() ;
            bool isIdle();

            double getFitness() { return fitness; };
            double setFitness(double fit) { fitness = fit; return fitness; };

            static constexpr int inputCount() { return 6; } ;
            static constexpr int outputCount() { return 4; };

            bool move(Vec2i vec);

            void update() override;
            void save(json& json) const override;
            void load(const json& json) override;

            AntIA& operator=(const AntIA& en);

        private:
            Genome m_genome;
            FeedForwardNeuralNetwork m_network;
            double fitness = 0.0;
            Vec2i m_dir;
            Vec2i m_gridPos;
            int lastAction = -1; 
            int directionChanges = 0;
            int repeatCount = 0; 
            int wallHit = 0;
            int goodWallAvoidanceMoves = 0;
            int numberOfCheckpoints = 0;
            int stuckCount = 0;
            bool end = false;
            

            std::unordered_set<std::pair<int, int>, simu::pair_hash> visitedPositions;

    };

/*
    class AntIALab : public AntIA {
    int steps_count;  // Compteur d'étapes
    int max_steps;    // Nombre maximal d'étapes autorisées
    simu::Vec2i goalPos;

public:
    AntIALab(const Genome &genome, simu::Vec2i goal, int max_steps)
        : simu::AntIA(genome), goalPos(goal), max_steps(max_steps), steps_count(0) {}

    bool canAct() const {
        return steps_count < max_steps;
    }

    void update() override
    {
    
    
    }

    void act(simu::Grid &grid) {
        if (canAct()) {
            auto inputs = simu::get_game_state_lab(getPos(), goalPos, grid);
            auto outputs = getNetwork().activate(inputs);
            simu::perform_action_lab(outputs, *this);
            steps_count++;
        }
    }
};
*/

    class Test: public Entity
    {
        public:
            Test(const long id = -1) : Entity(id) {};
            Test(const long id, const Test& ant) : Entity(id, ant) {};

            virtual ~Test(){};

            const char* getType() const override { return "test"; };

            void update() override {};
            void draw() override {};

            Test& operator=(const Test& en) 
            {
                Entity::operator=(en); 
                m_test = en.m_test;
                return *this;
            };

        private:
            std::string m_test = "this is a test";
    };

    
}

#endif