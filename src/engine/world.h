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
#include "ant.h"

// #define TEMPLATE_CONDITION(T) std::enable_if_t<std::is_base_of<Entity, T>::value && !std::is_same<Entity, T>::value>
#define TEMPLATE_CONDITION(T) std::enable_if_t<std::is_base_of<Entity, T>::value>
#define CHECK_TEMPLATE_ST(T) static_assert(std::is_base_of<Entity, T>::value);// && !std::is_same<Entity, T>::value, "T doit etre une class fille de Entity");

namespace simu
{
    class WorldListener;


    // Liste des entités enregistrés 
    using entities_t = std::variant<AntIA, Test, DemoAnt>;

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
             * @param args Les paramètres du constructeur de l'entité
             * @return La position du premier élément ajouté. Renvoie end() si il y a une erreur (count <= 0)
             */
            template<class T, typename... Args, class = TEMPLATE_CONDITION(T)>
            std::vector<std::weak_ptr<T>> spawnEntities(size_t count, const Args&... args)
            {
                CHECK_TEMPLATE_ST(T)

                if(count <= 0)
                    return std::vector<std::weak_ptr<T>>();

                const size_t en_cnt = m_entities.size();

                // Redimensionne le vecteur pour ajouter "count" nouveaux éléments
                m_entities.resize(en_cnt + count); 
                std::vector<std::weak_ptr<T>> newlies(count);

                // Itère à partir de l'ancienne fin, jusqu'à la nouvelle fin
                for (size_t i = 0; i < count; ++i)
                {
                    auto en = std::make_shared<T>(m_entity_cnt, args...);
                    m_entities[en_cnt + i] = en;
                    newlies[i] = en;
                    m_entity_cnt++;
                }   

                // Retourne un vecteur de weak_ptr vers les nouvelles entités ajoutées
                return newlies;
            };
            
            /**
             * @brief Ajoute une entité dans la simulation
             * @param args Les paramètres du constructeur de l'entité
             * @return Un pointeur vers la fourmis nouvellement crée
             */
            template<class T, typename... Args, class = TEMPLATE_CONDITION(T)>
            std::weak_ptr<T> spawnEntity(Args... args)
            {
                CHECK_TEMPLATE_ST(T)

                std::shared_ptr<T> en = std::make_shared<T>(m_entity_cnt, args...);

                m_entities.push_back(en);
                m_entity_cnt++;
                
                return en;
            };

            template<class T, class = TEMPLATE_CONDITION(T)>
            std::weak_ptr<T> getEntity(unsigned long id)
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

            bool exist(unsigned long id) const;

            std::vector<std::weak_ptr<Entity>> getEntities()
            {
                return std::vector<std::weak_ptr<Entity>>(m_entities.begin(), m_entities.end());
            }

            /** @brief Supprime une entitié en utilisant l'algo binary search. 
             *  @param id L'ID de l'entité.
             *  @return Renvoie vrai si l'entité à été supprimé et que l'ID existe.
             * 
             *  @warning A ne pas utiliser dans un update d'une entitié. 
             */
            bool removeEntity(unsigned long id);

            /** @brief Supprime toutes les entités fournis en paramètres.
             *  @param beg Début de la liste
             *  @param end Fin de la lite
             *  @example std::vector<std::weak_ptr<Ant>> en; \
             *  getWorld().removeEntities(en.begin(), en.end())
             */
            template<typename T>
            void removeEntities(T beg, T end)
            {
                for(auto it = beg; it != end; it++)
                {
                    if(auto sp = it->lock())
                    {
                        removeEntity(sp->getId());
                    }
                }
            }

            void clearEntities();

            void setListener(std::shared_ptr<WorldListener> listener)
            {
                m_listener = listener;
            };

            Grid& getGrid() { return m_grid; };
            
            void init() override;
            void unload() override;

            void save(const std::string& file);
            void load(const std::string& file);

            /**
             * @brief Renvoie la position de la souris dans la grille
             */
            Vector2i mouseToGridCoord() const;

            /**
             * @brief Renvoie la position globale de la position dans la grille
             * @param pos Position d'une tuile dans la grille courante
             * @return Les coordonées par rapport au repère de la caméra 2D
             */
            Vec2f gridToWorld(Vector2i pos) const;

            /**
             * Positionne la caméra au centre de la grille
             */
            void centerCamera();

            /**
             * @brief Renvoie la tuile correspondante à celle sélectionner
             */
            Tile getSelectedTile() const;

            /**
             * @brief Cherche une entité à la position indiquée en prennant compte la largeur de l'entité.
             * @param pos Position globale.
             * @return Renvoie la première entité trouvée pendant l'itération.
             */
            std::weak_ptr<Entity> getEntityAt(Vec2f pos);

        private:
            World();

            void handleKeyboard();
            void handleMouse();

            void drawFrame() override;
            void drawUI() override;
            void updateTick() override;

            void drawEntityInfo();

            void drawInputWithError(const char* label, char* buf, size_t buf_size, const char* error);
        
            unsigned long m_entity_cnt;
            unsigned int m_seed;

            std::shared_ptr<WorldListener> m_listener;
            std::vector<std::shared_ptr<Entity>> m_entities;

            std::weak_ptr<Entity> m_selected_en;

            Grid m_grid;

            int m_cursorTileIndex;
            const std::array<const Tile, 3> m_cursorTiles = {GROUND, FOOD, PHEROMONE};

            bool m_showGenome;
            bool m_focus_en_gui;
    };

    class WorldListener
    {
        public:
            virtual ~WorldListener() {};

            // executé lors du chargement de la scene
            virtual void onInit() {};

            // executé lors de la fermeture de la scene
            virtual void onUnload() {};

            // executé à la fréquence de rafraichissement de l'écran
            virtual void onDraw() {};

            // executé à la frequence de mise à jour de la logique de la simulation
            virtual void onUpdate() {};

            // executé à la fréquence de l'UI (30 FPS)
            virtual void onDrawUI() {};
    };

    inline World& getWorld() { return simu::World::world; };


}

#endif