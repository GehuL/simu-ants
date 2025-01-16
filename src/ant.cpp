#include "ant.h"

#include "raylib.h"
#include "raymath.h"

#include "world.h"

using namespace simu;

Ant::Ant(const long id) : Entity(id) {}
Ant::Ant(const long id, const Ant& ant) : Entity(id, ant), m_life(ant.m_life), 
m_carried_object(ant.m_carried_object) {}
Ant::Ant(const long id, Vector2f position) : Entity(id, position) {}

void Ant::update() {}

void Ant::draw() 
{
    DrawRectangle(this->m_pos.x, this->m_pos.y, 5, 5, DARKPURPLE);

    if(m_carried_object.type != Type::AIR)
    {
        DrawRectangle(this->m_pos.x, this->m_pos.y, 3, 3, m_carried_object.color);
    }
}

void Ant::save(json &json) const
{
    Entity::save(json);
    json["life"] = m_life;
    json["carried_object"] = m_carried_object.type;
}

void Ant::load(const json &json)
{
    Entity::load(json);
    json.at("life").get_to(m_life);
    json.at("carried_object").get_to(m_carried_object.type);
}

void Ant::rotate(float angle)
{
    m_velocity = Vector2Rotate((Vector2) {1.0, 0.0}, angle);
}

void Ant::move(Direction dir)
{
    rotate(dir);
    moveForward();
}

bool Ant::moveForward()
{
    Vec2f newPos = m_pos + m_velocity;
    Tile tile = getWorld().getGrid().getTile(newPos);

    if(!tile.flags.solid)
    {
        m_pos = newPos;
        return true;
    }
    return false;
}

void Ant::eat()
{
    Tile tile = getWorld().getGrid().getTile(m_pos);
    
    if(tile.flags.eatable)
    {
       if(m_life < 100.f)
            m_life += 10.f;

        getWorld().getGrid().setTile(AIR, m_pos.x, m_pos.y);
    }
}

void Ant::pheromone()
{
    Tile tile = getWorld().getGrid().getTile(m_pos);
    if(!tile.flags.solid)
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
    Tile facingTile = grid.getTile(facingTilePos);

    if(facingTile.flags.carriable)
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
    Tile facingTile = grid.getTile(facingTilePos);

    if(!facingTile.flags.solid)
    {
        grid.setTile(this->m_carried_object, facingTilePos.x, facingTilePos.y);
        this->m_carried_object = AIR;
    }     
}

Ant& Ant::operator=(const Ant& ant)
{
    Entity::operator=(ant);
    m_life = ant.m_life; 
    m_carried_object = ant.m_carried_object;
    return *this;
}

// ==================[DEMO ANT]==================
DemoAnt::DemoAnt(const long id) : Ant(id) {}
DemoAnt::DemoAnt(const long id, const DemoAnt &ant) : Ant(id, ant),  m_rotateCd(ant.m_rotateCd) {}
DemoAnt::DemoAnt(const long id, Vector2f position) : Ant(id, position) {}

void DemoAnt::update()
{
    if(m_rotateCd-- <= 0)
    {
        m_rotateCd = GetRandomValue(30, 100);
        m_angle += GetRandomValue(-100, 100) * 0.01f * PI / 4; // Rotation de +- 45°
        rotate(m_angle);
    }
    
    if(!moveForward())
        m_rotateCd = 0;


    if(isCarrying())
    {
        if(GetRandomValue(0, 100) == 0)
            put();
    }else if(getTileFacing().flags.carriable)
    {
        if(GetRandomValue(0, 100) == 0)
            take();
    }

    pheromone(); 
}

void DemoAnt::save(json &json) const
{
    Ant::save(json);
    json["rotateCd"] = m_rotateCd;
}

void DemoAnt::load(const json &json)
{
    Ant::load(json);
    json.at("rotateCd").get_to(m_rotateCd);
}

DemoAnt& DemoAnt::operator=(const DemoAnt& ant)
{
    Ant::operator=(ant);
    m_rotateCd = ant.m_rotateCd;
    return *this;
}

// ==================[ANT IA]==================
AntIA::AntIA(const long id) : Ant(id) {}
AntIA::AntIA(const long id, const AntIA& ant) : Ant(id, ant) {}
AntIA::AntIA(const long id, Vec2i position): Ant(id), m_gridPos(position) {}

void AntIA::save(json &json) const
{
    Ant::save(json);
    // TODO: Save genome
}

bool AntIA::move(Vec2i dir)
{
    Vec2i newPos = m_gridPos + dir;
    Tile tile = getWorld().getGrid().getTile(newPos);

    if(!tile.flags.solid)
    {
        m_gridPos = newPos;
        m_pos = getWorld().gridToWorld(newPos);
        return true;
    }
    return false;
}

void AntIA::update()
{
    if(m_rotateCd-- <= 0)
    {
        m_rotateCd = GetRandomValue(1, 100);

        static const Vec2i dirs[] = {LEFT, RIGHT, UP, DOWN}; 
        Vec2i dir = dirs[GetRandomValue(0, 4)];
        m_dir = dir;
    }

    if(!move(m_dir))
        m_rotateCd = 0;

    pheromone(); 
    // TODO: Activate neurones
}

void AntIA::load(const json &json)
{
    Ant::load(json);
    // TODO: Load genome
}

AntIA& AntIA::operator=(const AntIA& ant)
{
    Ant::operator=(ant);
    // TODO: Copy genome
    return *this;
}