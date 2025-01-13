#include "entity.h"
#include "world.h"

#include "raymath.h"

using namespace simu;

Entity::Entity(const long id, const Entity& en) : m_id(id), m_pos(en.m_pos), 
m_velocity(en.m_velocity), m_angle(en.m_angle) {}

Entity::Entity(const long id, Vector2f position): m_id(id), m_pos(position) {}
Entity::Entity(const long id) : m_id(id), m_angle(0) {}

Entity::~Entity()
{
    // std::cout << "Destroyed " << *this << std::endl;
}

Tile Entity::getTileOn() const
{
    return getWorld().getGrid().getTile(m_pos);
}

Tile Entity::getTileFacing() const
{
    Vector2i facingTilePos = getTileFacingPos();
    return getWorld().getGrid().getTile(facingTilePos);
}

Tile simu::Entity::getTileLeft() const
{
    Vector2i leftTilePos = getTileLeftPos();
    return getWorld().getGrid().getTile(leftTilePos);
}

Tile simu::Entity::getTileRight() const
{
    Vector2i rightTilePos = getTileRightPos();
    return getWorld().getGrid().getTile(rightTilePos);
}

Tile simu::Entity::getTileBack() const
{
    Vector2i backTilePos = getTileBackPos();
    return getWorld().getGrid().getTile(backTilePos);
}


Vector2i simu::Entity::getTileFacingPos() const
{
    Grid& grid = getWorld().getGrid();
    // Translate ant position to facing tile position
    Vector2i tilePos = grid.toTileCoord(m_pos.x + grid.getTileSize() / 2.0, m_pos.y + grid.getTileSize() / 2.0);
    Vector2 vel = Vector2Rotate((Vector2){1.0, 0}, m_angle);
    
    // Get the tile the ant is facing
    return (Vector2i){tilePos.x + static_cast<int>(round(vel.x)), tilePos.y + static_cast<int>(round(vel.y))};;
}

Vector2i simu::Entity::getTileLeftPos() const
{
    Grid& grid = getWorld().getGrid();
    // Translate ant position to facing tile position
    Vector2i tilePos = grid.toTileCoord(m_pos.x + grid.getTileSize() / 2.0, m_pos.y + grid.getTileSize() / 2.0);
    Vector2 vel = Vector2Rotate((Vector2){1.0, 0}, m_angle - PI / 2); // Rotate 90 degrees counterclockwise (left)
    
    // Get the tile to the left of the ant
    return (Vector2i){tilePos.x + static_cast<int>(round(vel.x)), tilePos.y + static_cast<int>(round(vel.y))};
}

Vector2i simu::Entity::getTileRightPos() const
{
    Grid& grid = getWorld().getGrid();
    // Translate ant position to facing tile position
    Vector2i tilePos = grid.toTileCoord(m_pos.x + grid.getTileSize() / 2.0, m_pos.y + grid.getTileSize() / 2.0);
    Vector2 vel = Vector2Rotate((Vector2){1.0, 0}, m_angle + PI / 2); // Rotate 90 degrees clockwise (right)
    
    // Get the tile to the right of the ant
    return (Vector2i){tilePos.x + static_cast<int>(round(vel.x)), tilePos.y + static_cast<int>(round(vel.y))};
}

Vector2i simu::Entity::getTileBackPos() const
{
    Grid& grid = getWorld().getGrid();
    // Translate ant position to facing tile position
    Vector2i tilePos = grid.toTileCoord(m_pos.x + grid.getTileSize() / 2.0, m_pos.y + grid.getTileSize() / 2.0);
    Vector2 vel = Vector2Rotate((Vector2){1.0, 0}, m_angle + PI); // Rotate 180 degrees (backwards)
    
    // Get the tile behind the ant
    return (Vector2i){tilePos.x + static_cast<int>(round(vel.x)), tilePos.y + static_cast<int>(round(vel.y))};
}


Vector2i simu::Entity::getTilePosOn() const
{
    return getWorld().getGrid().toTileCoord(m_pos.x, m_pos.y);
}

std::string Entity::toString() const
{
    return "{type:" + std::string(getType()) + ", id:" + std::to_string(m_id) + "}";
}

void Entity::save(json &j) const
{
    j["type"] =  getType(); 
    j["posX"] =  m_pos.x;
    j["posY"] =  m_pos.y;
    j["velX"] =  m_velocity.y;
    j["velY"] =  m_velocity.y;
    j["angle"] =  m_angle;
}

void Entity::load(const json &j)
{
    j.at("posX").get_to(m_pos.x);
    j.at("posY").get_to(m_pos.y);
    j.at("velX").get_to(m_velocity.y);
    j.at("velY") .get_to(m_velocity.y);
    j.at("angle").get_to(m_angle);
}

std::ostream& simu::operator<<(std::ostream& os, Entity& entity)
{
    os << entity.toString();
    return os;
}

Entity& Entity::operator=(const Entity& en)
{
    m_pos = en.m_pos;
    m_velocity = en.m_velocity;
    m_angle = en.m_angle;
    return *this;
}
