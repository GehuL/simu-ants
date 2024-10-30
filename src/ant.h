#ifndef __ANT_H__
#define __ANT_H__

#include <map>
#include <string>

#include "entity.h"
#include "tiles.h"

#include "../NEAT/Genome.h"
#include "../NEAT/NeuralNetwork.h"

namespace simu
{
    class Ant : public Entity
    {
        public:

            Ant(const long id = -1);
            Ant(const long id, const Ant& ant);

            virtual ~Ant(){};

            void update() override;
            void draw() override;

            void save(json& json) const override;
            void load(const json& json) override;

            Tile getCarriedObject() const { return m_carried_object; };

            float getLife() const { return m_life; };

            std::string getType() const override { return "ant"; };

            // ------ ACTIONS IA -------
            void rotate(float angle);  // Définie la direction et le sens de la fourmis
            void moveForward();        // Se déplace devant elle (en fonction de son angle)
            void eat();                // Mange sur sa position (si il y a quelque chose)
            void pheromone();          // Pose un phéromone sur sa position
            void beat();               // Mord l'entité devant elle

            void take();               // Porte un objet sur elle (nourriture/mur)
            void put();                // Déposer l'objet qu'elle porte

            Ant& operator=(const Ant& en);

        private:
            float m_life = 100.0;
            Tile m_carried_object = AIR; 

            float m_target_angle;
            int m_rotateCd;
    };

    class Test: public Entity
    {
        public:
            Test(const long id = -1) : Entity(id) {};
            Test(const long id, const Test& ant): Entity(id) {};

            virtual ~Test(){};

            std::string getType() const override { return "test"; };

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

    class AntController
    {
        public:
            AntController(const std::weak_ptr<Ant> ant);
            AntController(const std::weak_ptr<Ant> ant, const Genome& genome);
            
            virtual ~AntController(){};

            void activate();

        private:
            const std::weak_ptr<Ant> m_ant;
            FeedForwardNeuralNetwork m_network;
    };
}

#endif