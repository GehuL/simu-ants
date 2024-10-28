#ifndef __ANT_H__
#define __ANT_H__

#include <map>
#include <string>

#include "entity.h"
#include "tiles.h"

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

            float m_life;
            Tile m_carried_object; 

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
}

#endif