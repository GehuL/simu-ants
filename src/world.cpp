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

    spawnEntities<Ant>(10, nullptr);
}

void World::save(std::ofstream &file)
{
    TRACELOG(LOG_INFO, "Saving simulation..");
    json j;
    for(auto& en : m_entities)
    {
        j += *en.get();
    }
    file << j;
    TRACELOG(LOG_INFO, "Saved !");
}

void World::load(std::ifstream &file)
{
    TRACELOG(LOG_INFO, "Loading simulation..");
    json j = json::parse(file);
    
    for(auto& e : j)
    {
        auto& a = *spawnEntity<Ant>().lock();
        from_json(e, a);
    }
    // auto en = spawnEntity<Ant>().lock().get();
    // from_json(j, *en);
    TRACELOG(LOG_INFO, "Loaded !");
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
    {
        auto file = std::ifstream("simu-save.json", std::ios_base::in);
        load(file);
        file.close();
    }
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

