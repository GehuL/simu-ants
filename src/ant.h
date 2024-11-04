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

            virtual ~Ant() {};

            void update() override;
            void draw() override;

            void save(json& json) const override;
            void load(const json& json) override;

            Tile getCarriedObject() const { return m_carried_object; };

            float getLife() const { return m_life; };

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
    };

    /** Implémentation du comportement par défaut sans IA de la fourmis pour une démonstration.
     */
    class DemoAnt: public Ant
    {
        public:
            DemoAnt(const long id = -1);
            DemoAnt(const long id, const DemoAnt& ant);
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
            AntIA(const long id = -1);
            AntIA(const long id, const AntIA& ant);
            virtual ~AntIA() {};

            const char* getType() const override { return "antIA"; };

            void update() override;
            void save(json& json) const override;
            void load(const json& json) override;

            AntIA& operator=(const AntIA& en);

        private:
            // TODO: Ajouter neural network
    };

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