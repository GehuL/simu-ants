#include "world.h"
#include <exception>

#include "ant.h"

using namespace simu;

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
}

void World::updateTick()
{
    m_grid.update();
    for(auto& en : m_entities)
    {
        en->update();
    }
}

