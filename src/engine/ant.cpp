#include "ant.h"

#include "raylib.h"
#include "raymath.h"

#include "world.h"

using namespace simu;

Ant::Ant(const long id, const Ant& ant) : Entity(id, ant), m_life(ant.m_life), 
m_carried_object(ant.m_carried_object)
{
}

Ant::Ant(const long id) : Entity(id)
{
}

void Ant::update() {}

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

void Ant::moveForward()
{
    Vector2f lastPos = m_pos;
    m_pos = Vector2Add(m_pos, m_velocity);

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
    return *this;
}

// ==================[DEMO ANT]==================
DemoAnt::DemoAnt(const long id) : Ant(id) {}
DemoAnt::DemoAnt(const long id, const DemoAnt &ant) : Ant(id, ant),  m_rotateCd(ant.m_rotateCd) {}

void DemoAnt::update()
{
    if(m_rotateCd-- <= 0)
    {
        m_rotateCd = GetRandomValue(30, 100);
        m_angle += GetRandomValue(-100, 100) * 0.01f * PI / 4;
        rotate(m_angle);
    }
    
    Vector2 lastPos = m_pos;
    moveForward();

    // Tuile dans la direction de la fourmis
    Vector2i facingPos = getTileFacingPos();
    Tile facingTile = getWorld().getGrid().getTile(facingPos);

    if(facingTile.type == Type::BORDER || facingTile.type == Type::GROUND)
    {
        // Fait revenir a son ancienne position car elle risque de foncer dans un mur
        m_pos = lastPos;
        m_rotateCd = 0; // Elle prendra une nouvelle décision
    }

    // if(facingTile.type == Type::GROUND || facingTile.type == Type::FOOD)
    //     take();
    
    if(GetRandomValue(0, 40) == 0)
        put();

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
AntIA::AntIA(const long id) : Ant(id), m_rng(), m_network(FeedForwardNeuralNetwork::create_from_genome(Genome::create_genome(0, 3, 2, 3, m_rng))) {}
AntIA::AntIA(const long id, const AntIA& ant) : Ant(id, ant), m_network(ant.m_network) {}
AntIA::AntIA(const long id, const Genome& genome) : Ant(id), m_network(FeedForwardNeuralNetwork::create_from_genome(genome)){}

void AntIA::save(json &json) const
{
    Ant::save(json);
    // TODO: Save genome
}

void AntIA::update()
{
    // Variables de décisions
    const std::vector<double> inputs = {
    static_cast<double>(getAngle()), 
    static_cast<double>(getTileOn().type), 
    static_cast<double>(getTileFacing().type)};

    // Activation des sorties
    auto outputs = m_network.activate(inputs);

    // Décisions
    rotate(outputs[0]);
    moveForward();
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