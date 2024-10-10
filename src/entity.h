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

            Entity(const long id, const Entity& entity);
            Entity(const long id);

            virtual ~Entity();

            virtual void update() = 0;
            virtual void draw() = 0;

            virtual const std::string getType() const = 0;

            Tile getTileOn() const;
            Tile getTileFacing() const;

            Vector2i getTileFacingPos() const;
            Vector2i getTilePosOn() const;

            const long getId() const { return m_id; };

            Vector2 getPos() const{ return m_pos; };
            Vector2 getVel() const { return m_velocity; };

            const float getAngle() const { return m_angle; };

            std::string toString() const;

            friend void to_json(json& j, const Entity& p);
            friend void from_json(const json& j, Entity& p);

            Entity& operator=(const Entity& en);

        private:
            const long m_id;
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