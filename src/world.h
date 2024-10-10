#ifndef __WORLD_H__
#define __WORLD_H__


#include "engine.h"
#include "entity.h"
#include "tiles.h"

#include <vector>
#include <memory>
#include <optional>
#include <algorithm>
#include <array>
#include <fstream>

namespace simu
{
    class World : public Engine
    {
        public:
    
            static World world;

            template<class T>
            std::vector<std::shared_ptr<simu::Entity>>::iterator spawnEntities(size_t count)
            {
                const size_t en_cnt = m_entities.size();

                // Redimensionne le vecteur pour ajouter "count" nouveaux éléments
                m_entities.resize(en_cnt + count); 

                // Itère à partir de l'ancienne fin, jusqu'à la nouvelle fin
                for (size_t i = en_cnt; i < m_entities.size(); ++i)
                {
                    m_entities[i] = std::make_shared<T>(m_entity_cnt);
                    m_entity_cnt++;
                }

                // Retourne l'itérateur correspondant à l'ancienne fin (avant ajout des nouveaux éléments)
                return m_entities.begin() + en_cnt;
            }

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
            
            void init() override;

            void save(std::ofstream& file);
            void load(std::ifstream& file);

        private:
            World();

            void handleKeyboard();
            void handleMouse();

            void drawFrame() override;
            void drawUI() override;
            void updateTick() override;
        
            long m_entity_cnt;
            std::vector<std::shared_ptr<Entity>> m_entities;

            Grid m_grid;

            int m_cursorTileIndex;
            const std::array<const Tile, 3> m_cursorTiles = {GROUND, FOOD, PHEROMONE};  
    };

    inline World& getWorld() { return simu::World::world; };
}

#endif