#ifndef __WORLD_H__
#define __WORLD_H__


#include "engine.h"
#include "entity.h"
#include "tiles.h"

#include <vector>
#include <memory>
#include <optional>
#include <algorithm>

namespace simu
{
    class World : public Engine
    {
        public:
    
            static World world;

            template<class T>
            std::weak_ptr<T> spawnEntity()
            {
                 if(typeid(T) == typeid(Entity))
                    throw std::invalid_argument("T does not inherit Entity");

                std::shared_ptr<T> en = std::make_shared<T>(m_entity_cnt);

                m_entities.push_back(en);
                m_entity_cnt++;
                
                return en;
            };

            template<class T>
            std::weak_ptr<T> getEntity(long id)
            {
                auto it = std::find_if(m_entities.begin(), m_entities.end(), [id](std::shared_ptr<Entity> en)
                {
                    return en.get()->getId() == id;
                });

                std::weak_ptr<T> ptr;
                if(it != m_entities.end())
                {
                    ptr = std::dynamic_pointer_cast<T>(*it);
                }
                return ptr;
            };

            bool const exist(long id)
            {
                auto it = std::find_if(m_entities.begin(), m_entities.end(), [id](std::shared_ptr<Entity> en)
                {
                    return (bool)en && en.get()->getId() == id;
                });
                return it != m_entities.end() ;
            }

            Grid& getGrid() { return m_grid; };

        private:
            World();

            void init();

            void handleKeyboard();
            void handleMouse();

            void drawFrame() override;
            void drawUI() override;
            void updateTick() override;
        
            long m_entity_cnt;
            std::vector<std::shared_ptr<Entity>> m_entities;

            Grid m_grid;
    };

    inline World& getWorld() { return simu::World::world; };
}

#endif