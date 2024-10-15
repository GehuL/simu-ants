#ifndef __WORLD_H__
#define __WORLD_H__

#include <type_traits>
#include <vector>
#include <memory>
#include <optional>
#include <algorithm>
#include <array>
#include <fstream>
#include <variant>

#include "engine.h"
#include "entity.h"
#include "tiles.h"

#include "entity.h"
#include "ant.h"

// #define TEMPLATE_CONDITION(T) std::enable_if_t<std::is_base_of<Entity, T>::value && !std::is_same<Entity, T>::value>
#define TEMPLATE_CONDITION(T) std::enable_if_t<std::is_base_of<Entity, T>::value>
#define CHECK_TEMPLATE_ST(T) static_assert(std::is_base_of<Entity, T>::value);// && !std::is_same<Entity, T>::value, "T doit etre une class fille de Entity");

namespace simu
{
    // Liste des entités enregistrés 
    using entities_t = std::variant<Ant, Test>;

    template<size_t index = std::variant_size_v<entities_t>>
    entities_t entityFactory(const std::string& entity_type) {
        if constexpr (index == 0) {
            throw std::runtime_error("Impossible de charger l'entité " + entity_type);
        } else {
            // On récupère le type courant du variant
            using current_type = std::variant_alternative_t<index - 1, entities_t>;

            // Si le type courant correspond à entity_type, on retourne une instance de ce type
            if (current_type().getType() == entity_type) {
                return current_type();
            } else {
                // Sinon, on continue la récursion avec l'index précédent
                return entityFactory<index - 1>(entity_type);
            }
        }
    }

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
            std::vector<std::weak_ptr<simu::Entity>> spawnEntities(size_t count)
            {
                CHECK_TEMPLATE_ST(T)

                if(count <= 0)
                    return std::vector<std::weak_ptr<Entity>>();

                const size_t en_cnt = m_entities.size();

                // Redimensionne le vecteur pour ajouter "count" nouveaux éléments
                m_entities.resize(en_cnt + count); 
                std::vector<std::weak_ptr<Entity>> newlies(m_entities.size());

                // Itère à partir de l'ancienne fin, jusqu'à la nouvelle fin
                for (size_t i = en_cnt; i < m_entities.size(); ++i)
                {
                    m_entities[i] = std::make_shared<T>(m_entity_cnt);
                    newlies[i] = m_entities[i];
                    m_entity_cnt++;
                }

                // Retourne l'itérateur correspondant à l'ancienne fin (avant ajout des nouveaux éléments)
                return newlies;
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
            void unload() override;

            void save(const std::string& file);
            void load(const std::string& file);

        private:
            World();

            void handleKeyboard();
            void handleMouse();

            void drawFrame() override;
            void drawUI() override;
            void updateTick() override;
        
            long m_entity_cnt;
            unsigned int m_seed;

            std::vector<std::shared_ptr<Entity>> m_entities;

            Grid m_grid;

            int m_cursorTileIndex;
            const std::array<const Tile, 3> m_cursorTiles = {GROUND, FOOD, PHEROMONE};  
    };

    inline World& getWorld() { return simu::World::world; };
}

#endif