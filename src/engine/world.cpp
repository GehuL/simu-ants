#include "world.h"
#include <exception>

#include "utils.h"
#include "ant.h"
#include "../external/json.hpp"

using namespace simu;
using json = nlohmann::json;

World World::world;

World::World() : m_entity_cnt(0), m_grid(5)
{

}

void World::init()
{
    Engine::init();

    m_seed = GetRandomValue(0, std::numeric_limits<int>::max());
    SetRandomSeed(m_seed);

    TRACELOG(LOG_INFO, "seed: %d", m_seed);
    m_entities.clear();

    if(m_listener)
        m_listener.get()->onInit();
}

void World::unload()
{
    if(m_listener)
        m_listener.get()->onUnload();
    m_grid.unload();
}

Vector2i World::mouseToGridCoord() const
{
    Vector2f pos = GetScreenToWorld2D(GetMousePosition(), m_camera);
    return m_grid.toTileCoord(pos.x, pos.y);
}

Vector2f World::gridCoordToWorld(Vector2i pos) const
{
    return Vector2f{static_cast<float>(pos.x * m_grid.getTileSize()), static_cast<float>(pos.y * m_grid.getTileSize())};
}

void World::centerCamera()
{
    float offset = (GetScreenWidth() - m_grid.getGridWidth() * m_grid.getTileSize()) / 2.f;
    m_camera.offset = (Vector2){offset, offset};
}

Tile World::getSelectedTile() const
{
    return m_cursorTiles[m_cursorTileIndex];
}

void World::save(const std::string& filename)
{
    TRACELOG(LOG_INFO, "Saving simulation..");
 
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
        j["seed"] = m_seed;

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
                std::visit([&](auto& e) mutable {
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
        
        if(j.find("seed") != j.end())
        {
            m_seed = j.at("seed");
            SetRandomSeed(m_seed);
        }

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
    // TODO: Interpoler les points pour tracer des lignes
    Vector2 pos = GetScreenToWorld2D(GetMousePosition(), m_camera);
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) // PUT WALL
    {
        m_grid.setTile(getSelectedTile(), pos.x, pos.y);
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
    constexpr float camera_speed = 5.0f; 

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

    // Coordonnées de la souris
    Vector2i mousePos = mouseToGridCoord();
    DrawText(TextFormat("(X: %d|Y: %d)", mousePos.x, mousePos.y), 5, GetScreenHeight() - 20, 16, GRAY);
    
    // Type de tuile sélectionné
    DrawText("Tile: ", 100, GetScreenHeight() - 20, 16, GRAY);
    DrawRectangle(135, GetScreenHeight() - 17, 10, 10, getSelectedTile().color);

    // Nombre d'entités
    DrawText(TextFormat("Entity: %d", m_entities.size()), 0, 100, 20, BLUE);
}

void World::updateTick()
{
    m_grid.update();
    for(auto& en : m_entities)
    {
        en->update();
    }

    if(m_listener)
        m_listener.get()->onUpdate();
}

