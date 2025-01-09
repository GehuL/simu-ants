#include "ant.h"

#include "raylib.h"
#include "raymath.h"

#include "world.h"

#include <random> 

using namespace simu;

Ant::Ant(const long id) : Entity(id) {}
Ant::Ant(const long id, const Ant& ant) : Entity(id, ant), m_life(ant.m_life), 
m_carried_object(ant.m_carried_object)
{
}
Ant::Ant(const long id, Vec2f pos) : Entity(id, pos) {}

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
        //std::cout << "Nouvelle position : (" << m_pos.x << ", " << m_pos.y << ")" << std::endl;
        return true;
    }
    //std::cout << "Déplacement bloqué à : (" << newPos.x << ", " << newPos.y << ")" << std::endl;
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
RNG gRng;

AntIA::AntIA(const long id) : Ant(id), m_genome(Genome::create_genome(0, 5, 4, 3, gRng)), m_network(FeedForwardNeuralNetwork::create_from_genome(m_genome)) {}
AntIA::AntIA(const long id, const AntIA& ant) : Ant(id, ant), m_network(ant.m_network), m_genome(ant.m_genome) {}
AntIA::AntIA(const long id, const Genome genome) : Ant(id), m_genome(genome), m_network(FeedForwardNeuralNetwork::create_from_genome(genome)) {}

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
    static_cast<double>(getTileFacing().type),
    static_cast<double>(getPos().x),
    static_cast<double>(getPos().y)
    };
/*
    std::cout << "Inputs: ";
for (double input : inputs) {
    std::cout << input << " ";
}
std::cout << std::endl;
*/

    // Activation des sorties
    auto actions = m_network.activate(inputs);

    // Ajouter du bruit aléatoire aux actions
    std::random_device rd;  // Générateur aléatoire
    std::mt19937 gen(rd()); // Mersenne Twister
    std::uniform_real_distribution<> dis(-0.1, 0.1); // Bruit entre -0.1 et 0.1

    for (double &action : actions)
    {
        action += dis(gen); // Ajouter un bruit aléatoire
    }


/*
    std::cout << "Outputs: ";
for (double action : actions) {
    std::cout << action << " ";
}
std::cout << std::endl;
*/
    // Décisions
    int direction = std::distance(actions.begin(), std::max_element(actions.begin(), actions.end()));

    //std::cout << "Ant " << getId() << " action: " << direction << std::endl;


    switch (direction) {
        case 0: move(EAST);  break;
        case 1: move(WEST);  break;
        case 2: move(NORTH); break;
        case 3: move(SOUTH); break;
        default: break;
    }
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