#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <string>
#include <ostream>
#include <iostream>

#include "json.hpp"

#include "raylib.h"
#include "tiles.h"
#include "others.h"

using json = nlohmann::json;

namespace simu
{
    class Entity
    {
        public:
            friend class World;

            Entity(const long id, const std::string type = "entity");

            virtual ~Entity();

            virtual void update() = 0;
            virtual void draw() = 0;

            Tile const getTileOn();
            Tile const getTileFacing();

            Vector2i const getTileFacingPos();
            Vector2i const getTilePosOn();

            long const getId() { return m_id; };

            Vector2 const getPos() { return m_pos; };
            Vector2 const getVel() { return m_velocity; };

            float const getAngle() { return m_angle; };

            std::string const getType() { return m_type; };
            std::string const toString();

            friend void to_json(json& j, const Entity& p);
            friend void from_json(const json& j, Entity& p);

        private:
            const long m_id;
            std::string m_type;
        protected:
            Vector2 m_pos;
            Vector2 m_velocity;
            float m_angle;
    };

    void to_json(json& j, const Entity& p);
    void from_json(const json& j, Entity& p);

    /*class Carriable
    {
        public:
            virtual void onCarry();
            virtual void onDropped();
        
            Entity& const getCarrier();
            void setCarrier(Entity& carrier);

            bool const isCarried();

        private:
            std::weak_ptr<Entity> m_carrier;
            bool is_carried;
    };*/

    std::ostream &operator<<(std::ostream& os, Entity& entity);
}

#endif