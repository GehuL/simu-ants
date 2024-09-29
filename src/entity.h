#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <string>
#include <ostream>
#include <iostream>

namespace simu
{
    class Entity
    {
        public:
            friend class World;

            Entity(const std::string type = "entity");

            virtual ~Entity();

            virtual void update() = 0;
            virtual void draw() = 0;

            long const getId() { return m_id; };

            std::string const getType() { return m_type; };
            std::string const toString();

        private:
            const std::string m_type;
            long m_id;
    };
    std::ostream &operator<<(std::ostream& os, Entity& entity);
}

#endif