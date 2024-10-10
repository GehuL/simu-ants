#ifndef __WORLD_H__
#define __WORLD_H__

#include <type_traits>
#include <vector>
#include <memory>
#include <optional>
#include <algorithm>
#include <array>
#include <fstream>

#include "engine.h"
#include "entity.h"
#include "tiles.h"

#define TEMPLATE_CONDITION(T) std::enable_if_t<std::is_base_of<Entity, T>::value && !std::is_same<Entity, T>::value>
#define CHECK_TEMPLATE_ST(T) static_assert(std::is_base_of<Entity, T>::value && !std::is_same<Entity, T>::value, "T doit etre une class fille de Entity");

namespace simu
{
    class World : public Engine
    {
        public:
    
            static World world;

            /**
             * @brief Ajoute des entités dans la simulation
             * @param count Quantité d'entitée à ajouter
             * @return La position du premier élément ajouté. Renvoie end() si il y a une erreur (count <= 0)
             */
            template<class T, class = TEMPLATE_CONDITION(T)>
            std::vector<std::shared_ptr<simu::Entity>>::iterator spawnEntities(size_t count)
            {
                CHECK_TEMPLATE_ST(T)

                if(count <= 0)
                    return m_entities.end();

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

            template<class T, class = TEMPLATE_CONDITION(T)>
            std::weak_ptr<T> spawnEntity()
            {
                CHECK_TEMPLATE_ST(T)

                std::shared_ptr<T> en = std::make_shared<T>(m_entity_cnt);

                m_entities.push_back(en);
                m_entity_cnt++;
                
                return en;
            };

            template<class T, class = TEMPLATE_CONDITION(T)>
            std::weak_ptr<T> getEntity(long id)
            {
                CHECK_TEMPLATE_ST(T)

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
            void load(const std::string& file);

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