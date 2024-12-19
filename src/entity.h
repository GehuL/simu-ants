#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <string>
#include <ostream>
#include <iostream>

#include "external/json.hpp"

#include "raylib.h"
#include "tiles.h"
#include "types.h"
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
            Entity(const long id, Vector2f position);

            virtual ~Entity();

            virtual void update() {};
            virtual void draw() {};

            virtual void save(json& json) const;
            virtual void load(const json& json);
        
            friend void to_json(json& j, const Entity& p) { p.save(j); };
            friend void from_json(const json& j, Entity& p) { p.load(j); };
            
            virtual const char* getType() const = 0;

            void setPos(Vector2f pos) { this-> m_pos = pos; };

            Tile getTileOn() const;
            Tile getTileFacing() const;

            Vector2i getTileFacingPos() const;
            Vector2i getTilePosOn() const;

            const long getId() const { return m_id; };

            Vec2f getPos() const { return m_pos; };
            Vec2f getVel() const { return m_velocity; };

            const float getAngle() const { return m_angle; };

            std::string toString() const;
            Entity& operator=(const Entity& en);

        private:
            const long m_id;
        protected:
            Vec2f m_pos = Vector2{0.f, 0.f};
            Vec2f m_velocity = Vector2{0.f, 0.f};
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