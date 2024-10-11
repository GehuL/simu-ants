#include "world.h"
#include <exception>

#include "utils.h"
#include "ant.h"
#include "json.hpp"

using namespace simu;
using json = nlohmann::json;

World World::world;

World::World() : m_entity_cnt(0), m_grid(100, 5)
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
    spawnEntities<Test>(10);
}

void World::save(std::ofstream &file)
{
    TRACELOG(LOG_INFO, "Saving simulation..");
 
    // TODO: Traiter les exceptions
    // TODO: Sauvegarder n'importe quel type enfant de Entity
    // TODO: Sauvegarder la seed de la génération de nombre aléatoire
    // TODO: Sauvegarder la grille
    json j;
    for(auto& en : m_entities)
    {
        Entity& a = *en.get();
        json ja;
        a.save(ja);
        j += ja;
    }
    file << j;
    TRACELOG(LOG_INFO, "Saved !");
}

void World::load(const std::string& filename)
{
    TRACELOG(LOG_INFO, "Loading simulation..");
    
    // TODO: Charger n'importe quel type enfant de Entity
    // TODO: Charger la seed de la génération de nombre aléatoire
    // TODO: Charger la grille
    try
    {
        auto file = std::ifstream(filename, std::ios_base::in);
        
        json j = json::parse(file);
        m_entities.clear(); // On peut clear si pas d'exception avant

        for(size_t i = 0; i < j.size(); i++)
        {
            std::string typestr = j[i]["type"];
            entities_t t = entityFactory(typestr);
            std::visit([=](auto e) mutable {
                std::cout << e << std::endl;
                 this->spawnEntity<decltype(e)>();
            }, t);
        }

        file.close();

        TRACELOG(LOG_INFO, "Loaded !");
    }catch(const json::exception& e)
    {
        TRACELOG(LOG_ERROR, "Erreur de chargement du fichier %s: %s", filename, e.what());
    }
    catch(const std::ifstream::failure& e)
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
    {
        auto file = std::ofstream("simu-save.json", std::ios_base::out);
        save(file);
        file.close();
    }

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

