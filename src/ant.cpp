#include "ant.h"

#include "raylib.h"
#include "raymath.h"

#include "world.h"

using namespace simu;

Ant::Ant(const long id) : Entity(id, "ant"), m_life(100.f), m_carried_object(AIR)
{
    m_velocity = (Vector2){1.f, 1.f};
}

void Ant::update()
{
    // if(m_life <= 0)
    //     return;    
 
    // m_life -= 0.01f;

    // if(m_rotateCd <= 0)
    // {
    //     m_rotateCd = 100;
    //     m_target_angle = GetRandomValue(-100, 100) * 0.01f * 2 * PI;
    // }
    // m_rotateCd--;

    // float da = m_target_angle - m_angle;
    // m_angle += da * 0.00001f;
    // m_angle = -PI/2.f;

    // move();
    this->m_pos.x += 0.1f;

    pheromone();

    take();
}

void Ant::draw() 
{
    DrawRectangle(this->m_pos.x, this->m_pos.y, 5, 5, DARKBROWN);

    if(m_carried_object.type != Type::AIR)
     {
        DrawRectangle(this->m_pos.x, this->m_pos.y, 3, 3, m_carried_object.color);
     }
}

void simu::Ant::eat()
{
    Tile tile = getWorld().getGrid().getTile(m_pos.x, m_pos.y);
    
    if(tile.type == Type::FOOD)
    {
       if(m_life < 100.f)
            m_life += 10.f;

        getWorld().getGrid().setTile(AIR, m_pos.x, m_pos.y);
    }
}

void Ant::pheromone()
{
    Tile tile = getWorld().getGrid().getTile(m_pos.x, m_pos.y);
    if(tile.type == Type::AIR || tile.type == Type::PHEROMONE)
    {
        getWorld().getGrid().setTile(PHEROMONE, m_pos.x, m_pos.y);
    }
}

void Ant::beat()
{
    // Prendre l'entité la plus proche
    // Vérifier si elle est en face
    // Retirer de la vie
}

void Ant::take()
{
    if(this->m_carried_object.type != Type::AIR)
        return;

    Grid& grid = getWorld().getGrid();

    Vector2i facingTilePos = getTileFacingPos();
    Tile facingTile = grid.getTile(static_cast<int>(facingTilePos.x), static_cast<int>(facingTilePos.y));

    if(facingTile.type == Type::FOOD || facingTile.type == Type::GROUND)
    {
        this->m_carried_object = facingTile;
        grid.setTile(AIR, facingTilePos.x, facingTilePos.y);
    }
}

void Ant::put()
{
    if(this->m_carried_object.type == Type::AIR)
        return;

    Grid& grid = getWorld().getGrid();

    Vector2i facingTilePos = getTileFacingPos();
    Tile facingTile = grid.getTile(facingTilePos.x, facingTilePos.y);

    if(facingTile.type == Type::AIR || facingTile.type == Type::PHEROMONE)
    {
        grid.setTile(this->m_carried_object, facingTilePos.x, facingTilePos.y);
        this->m_carried_object = AIR;
    }     
}

void Ant::move()
{
    m_velocity = Vector2Rotate(m_velocity, m_angle);
    m_pos = Vector2Add(m_pos, m_velocity);
}
