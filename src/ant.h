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

            Ant(const long id, const Ant& ant);
            Ant(const long id = -1);

            virtual ~Ant(){};

            void update() override;
            void draw() override;

            Tile const getCarriedObject() { return m_carried_object; };

            float const getLife() { return m_life; };

            // ------ ACTIONS IA -------
            void eat();       // Mange sur sa position (si il y a quelque chose)
            void pheromone(); // Pose un phéromone sur sa position
            void beat();      // Mord l'entité devant elle

            void take();     // Porte un objet sur elle (nourriture/mur)
            void put();       // Déposer l'objet qu'elle porte

            friend void to_json(json& j, const Ant& p);
            friend void from_json(const json& j, Ant& p);

            Ant& operator=(const Ant& en);

        private:

            void move(); // Se déplace en fonction de son angle

            float m_life;
            Tile m_carried_object; 

            float m_target_angle;
            int m_rotateCd;
    };

    void to_json(json& j, const Ant& p) ;
    void from_json(const json& j, Ant& p);
}

#endif