#include "world.h"
#include <exception>

#include "utils.h"
#include "ant.h"
#include "external/json.hpp"

using namespace simu;
using json = nlohmann::json;

World World::world;

World::World() : m_entity_cnt(0), m_grid(500, 5)
{
}

void World::init()
{
    Engine::init();

    float offset = (GetScreenWidth() - m_grid.getGridWidth() * m_grid.getTileSize()) / 2.f;
    m_camera.offset = (Vector2){offset, offset};

    m_entities.clear();
    m_grid.reset();

    spawnEntities<Ant>(10);
}

void World::save(const std::string& filename)
{
    TRACELOG(LOG_INFO, "Saving simulation..");
 
    // TODO: Sauvegarder la seed de la génération de nombre aléatoire
    // TODO: Sauvegarder la grille
    try
    {
        json j;

        for(auto& en : m_entities)
        {
            Entity& a = *en.get();
            json ja;
            a.save(ja);
            j["entities"] += ja;
        }

        j["grid"] = m_grid;

        auto file = std::ofstream(filename, std::ios_base::out);
        file << j;
        file.close();

        TRACELOG(LOG_INFO, "File saved to %s", filename.c_str());
    }catch(const json::exception& e)
    {
        TRACELOG(LOG_ERROR, "Erreur de chargement du fichier %s: %s", filename, e.what());
    } 
}

void World::load(const std::string& filename)
{
    TRACELOG(LOG_INFO, "Loading file %s", filename.c_str());
    
    // TODO: Optimiser la fonction
    // TODO: Charger la seed de la génération de nombre aléatoire
    // TODO: Charger la grille
    try
    {
        auto file = std::ifstream(filename, std::ios_base::in);
        
        json j = json::parse(file);
        
        // Lecture des entités
        auto entities_json = j.at("entities");
        std::vector<std::shared_ptr<Entity>> entities_tmp;

        if(entities_json.is_array())
        {
            entities_tmp.resize(entities_json.size());

            // Instantie la bonne class en fonction du type json
            for(size_t i = 0; i < entities_json.size(); i++)
            {
                std::string typestr = entities_json[i]["type"];
                entities_t t = entityFactory(typestr);
                std::visit([&](auto e) mutable {
                    using EntityType = std::decay_t<decltype(e)>;
                    auto en = std::make_shared<EntityType>(m_entity_cnt++);
                    auto en_j = entities_json[i];
                    en.get()->load(en_j);
                    entities_tmp[i] = en;
                }, t);
            }
        }

        // Lecture de la grille
        j.at("grid").get_to(m_grid);

        // Si on arrive ici c'est qu'il n'y a pas eu d'erreurs
        m_entities.clear();
        m_entities = std::move(entities_tmp); // On peut altérer la partie
       
        file.close();

        TRACELOG(LOG_INFO, "Loaded !");
    } catch(const json::exception& e)
    {
        TRACELOG(LOG_ERROR, "Erreur de chargement du fichier %s: %s", filename, e.what());
    } catch(const std::runtime_error& e)
    {
        TRACELOG(LOG_ERROR, "Erreur de chargement du fichier %s: %s", filename, e.what());
    }
}

void World::handleMouse()
{
    Vector2 pos = GetScreenToWorld2D(GetMousePosition(), m_camera);
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) // PUT WALL
    {
        m_grid.setTile(m_cursorTiles[m_cursorTileIndex], pos.x, pos.y);
    }else if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) // REMOVE TILE
    {
        m_grid.setTile(AIR, pos.x, pos.y);
    }else
    {
        Vector2i grid_pos = m_grid.toTileCoord(pos.x, pos.y);
        const int tileSize = m_grid.getTileSize();
        DrawRectangle(grid_pos.x * tileSize , grid_pos.y * tileSize, tileSize, tileSize, (Color){50, 0, 253, 100});
    }
    
    if(IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
    {
        m_cursorTileIndex++;
        m_cursorTileIndex %= m_cursorTiles.size();
    }

    m_camera.zoom += GetMouseWheelMove() * 0.1f;
}

void World::handleKeyboard()
{
    constexpr float camera_speed = 3.5f; 

    if (IsKeyDown(KEY_DOWN)) m_camera.offset.y -= camera_speed;
    if (IsKeyDown(KEY_UP)) m_camera.offset.y += camera_speed;
    if (IsKeyDown(KEY_RIGHT)) m_camera.offset.x -= camera_speed;
    if (IsKeyDown(KEY_LEFT)) m_camera.offset.x += camera_speed;

    if(IsKeyPressed(KEY_ENTER)) init();

    if(IsKeyPressed(KEY_P)) setPause(!isPaused());

    if(IsKeyPressed(KEY_S))
        save("simu-save.json");

    if(IsKeyPressed(KEY_R))
        load("simu-save.json");
}

void World::drawFrame()
{
    m_grid.draw();

    for(auto& en : m_entities)
    {
        en->draw();
    }
}

void World::drawUI()
{
    handleKeyboard();
    handleMouse();

    DrawText(TextFormat("Entity: %d", m_entities.size()), 0, 100, 20, BLUE);
}

void World::updateTick()
{
    m_grid.update();
    for(auto& en : m_entities)
    {
        en->update();
    }
}

