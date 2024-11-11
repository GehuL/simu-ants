#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <string>
#include <ostream>
#include <iostream>

#include "../external/json.hpp"

#include "raylib.h"
#include "tiles.h"
#include "others.h"
#include "variant"

using json = nlohmann::json;

namespace simu
{
    class Entity
    {
        public:
            friend class World;

            Entity(const long id = -1);
            Entity(const long id, const Entity& entity);

            virtual ~Entity();

            virtual void update() {};
            virtual void draw() {};

            virtual void save(json& json) const;
            virtual void load(const json& json);
        
            friend void to_json(json& j, const Entity& p) { p.save(j); };
            friend void from_json(const json& j, Entity& p) { p.load(j); };
            
            virtual const char* getType() const = 0;

            Tile getTileOn() const;
            Tile getTileFacing() const;

            Vector2i getTileFacingPos() const;
            Vector2i getTilePosOn() const;

            const long getId() const { return m_id; };

            Vector2 getPos() const { return m_pos; };
            Vector2 getVel() const { return m_velocity; };

            const float getAngle() const { return m_angle; };

            std::string toString() const;
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

    std::ostream &operator<<(std::ostream& os, Entity& entity);
   
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
}

#endif