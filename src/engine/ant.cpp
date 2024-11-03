#include "ant.h"

#include "raylib.h"
#include "raymath.h"

#include "world.h"

using namespace simu;

Ant::Ant(const long id, const Ant& ant) : Entity(id, ant), m_life(ant.m_life), 
m_carried_object(ant.m_carried_object), 
m_target_angle(ant.m_target_angle), m_rotateCd(ant.m_rotateCd)
{
}

Ant::Ant(const long id) : Entity(id)
{
}


void Ant::update()
{
}

void Ant::draw() 
{
    DrawRectangle(this->m_pos.x, this->m_pos.y, 5, 5, DARKBROWN);

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
    json["target_angle"] = m_target_angle;
    json["rotateCd"] = m_rotateCd;
}

void Ant::load(const json &json)
{
    Entity::load(json);
    json.at("life").get_to(m_life);
    json.at("carried_object").get_to(m_carried_object.type);
    json.at("target_angle").get_to(m_target_angle);
    json.at("rotateCd").get_to(m_rotateCd);
}

void Ant::rotate(float angle)
{
    m_velocity = Vector2Rotate((Vector2) {1.0, 0.0}, angle);
}

void Ant::moveForward()
{
    Vector2f lastPos = m_pos;
    m_pos = Vector2Add(m_pos, m_velocity);

    Tile tileOn = getTileOn();
    Tile tileAhead = getTileFacing();

    if(tileAhead.type == Type::BORDER ||
        tileAhead.type == Type::GROUND) 
        m_pos = lastPos;
}

void Ant::eat()
{
    Tile tile = getWorld().getGrid().getTile(m_pos);
    
    if(tile.type == Type::FOOD)
    {
       if(m_life < 100.f)
            m_life += 10.f;

        getWorld().getGrid().setTile(AIR, m_pos.x, m_pos.y);
    }
}

void Ant::pheromone()
{
    Tile tile = getWorld().getGrid().getTile(m_pos);
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
    Tile facingTile = grid.getTile(facingTilePos);

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
    Tile facingTile = grid.getTile(facingTilePos);

    if(facingTile.type == Type::AIR || facingTile.type == Type::PHEROMONE)
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
    m_target_angle = ant.m_target_angle;
    m_rotateCd = ant.m_rotateCd;
    return *this;
}

AntController::AntController(const std::weak_ptr<Ant> ant, const Genome& genome) : m_ant(ant), m_network(create_from_genome(genome)) {}
AntController::AntController(const std::weak_ptr<Ant> ant) : m_ant(ant), m_network(create_from_genome(Genome())) {}

void AntController::activate()
{
    if(!m_ant.expired())
    {
        auto ant = m_ant.lock();
        
        // Variables de décisions
        const std::vector<double> inputs = {
        static_cast<double>(ant->getAngle()), 
        static_cast<double>(ant->getTileOn().type), 
        static_cast<double>(ant->getTileFacing().type)};

        // Activation des sorties
        auto outputs = m_network.activate(inputs);

        // Décisions
        ant->rotate(outputs[0]);
        ant->moveForward();
    }
}