#include "entity.h"
#include "world.h"

#include "raymath.h"

using namespace simu;

Entity::Entity(const long id, const std::string type) : m_id(id), m_type(type),  m_angle(0) 
{
    m_pos = (Vector2) {0.f, 0.f};
    m_velocity = (Vector2) {0.f, 0.f};
};

Entity::~Entity()
{
    std::cout << "Destroyed " << *this << std::endl;
}

Tile const Entity::getTileOn()
{
    return getWorld().getGrid().getTile(m_pos.x, m_pos.y);
}

Tile const Entity::getTileFacing()
{
    Vector2i facingTilePos = getTileFacingPos();
    return getWorld().getGrid().getTile(facingTilePos.x, facingTilePos.y);;
}

Vector2i const simu::Entity::getTileFacingPos()
{
    Grid& grid = getWorld().getGrid();
    // Translate ant position to facing tile position
    Vector2i tilePos = grid.toTileCoord(m_pos.x, m_pos.y);
    Vector2 vel = Vector2Rotate((Vector2){1.0, 0}, m_angle);
    
    // Get the tile the ant is facing
    return (Vector2i){tilePos.x + static_cast<int>(vel.x), tilePos.y + static_cast<int>(vel.y)};;
}

std::string const Entity::toString()
{
    return "{type:" + m_type + ",id:" + std::to_string(m_id) + "}";
}

std::ostream& simu::operator<<(std::ostream& os, Entity& entity)
{
    std::string str = entity.toString();
    os << str;
    return os;
}