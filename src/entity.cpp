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
    Vector2i tilePos = grid.toTileCoord(m_pos.x + grid.getTileSize() / 2.0, m_pos.y + grid.getTileSize() / 2.0);
    Vector2 vel = Vector2Rotate((Vector2){1.0, 0}, m_angle);
    
    // Get the tile the ant is facing
    return (Vector2i){tilePos.x + static_cast<int>(round(vel.x)), tilePos.y + static_cast<int>(round(vel.y))};;
}

Vector2i const simu::Entity::getTilePosOn()
{
    return getWorld().getGrid().toTileCoord(m_pos.x, m_pos.y);
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

void simu::to_json(json& j, const simu::Entity& p) 
{
    j["type"] = p.m_type;
    j["posX"] = p.m_pos.x;
    j["posY"] = p.m_pos.y;
    j["velX"] = p.m_velocity.y;
    j["velY"] = p.m_velocity.y;
    j["angle"] = p.m_angle;
}

void simu::from_json(const json& j, simu::Entity& p)
{
    j.at("type").get_to(p.m_type);
    j.at("posX").get_to(p.m_pos.x);
    j.at("posY").get_to( p.m_pos.y);
    j.at("velX").get_to( p.m_velocity.y);
    j.at("velY") .get_to( p.m_velocity.y);
    j.at("angle").get_to( p.m_angle);
}