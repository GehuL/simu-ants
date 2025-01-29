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
RNG gRng;

AntIA::AntIA(const long id, const AntIA& ant) : Ant(id, ant), m_genome(ant.m_genome), m_network(ant.m_network) {}
AntIA::AntIA(const long id, Vec2i position): Ant(id),  m_genome(Genome::create_minimal_genome(19, 4, gRng)), m_network(FeedForwardNeuralNetwork::create_from_genome(m_genome)), m_gridPos(position)
{
    m_pos = getWorld().gridToWorld(position);
}

AntIA::AntIA(const long id, const Genome genome, Vec2i pos) : Ant(id), m_genome(genome), m_network(FeedForwardNeuralNetwork::create_from_genome(genome)), m_gridPos(pos) 
{
    m_pos = getWorld().gridToWorld(pos);
}

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
        return true;
    }
    return false;
}

void AntIA::update()
{
    m_pos = getWorld().gridToWorld(m_gridPos);

    // Variables de décisions
    const std::vector<double> inputs = {
    //static_cast<double>(getAngle()), 
    static_cast<double>(getTileFacing().flags.solid),
    static_cast<double>(getTileLeft().flags.solid),
    static_cast<double>(getTileRight().flags.solid),
    static_cast<double>(getTileBack().flags.solid),
    static_cast<double>(m_gridPos.x),
    static_cast<double>(m_gridPos.y),
    static_cast<double>(isStuck()),
    static_cast<double>(isIdle()),
    static_cast<double>(isCurrentPositionVisited()),
    static_cast<double>(getVisitedPositionsSize()),
    static_cast<double>(getDistanceToWall(UP)),
    static_cast<double>(getDistanceToWall(DOWN)),
    static_cast<double>(getDistanceToWall(LEFT)),
    static_cast<double>(getDistanceToWall(RIGHT)),
    static_cast<double>(getLastAction()),
    static_cast<double>(getDirectionChanges()),
    static_cast<double>(getRepeatCount()),
    static_cast<double>(getWallHit()),
    static_cast<double>(getGoodWallAvoidanceMoves()),
    };
/*
    std::cout << "Inputs: ";
for (double input : inputs) {
    std::cout << input << " ";
}
std::cout << std::endl;
*/

    std::vector<double> actions;
    try
    {
       actions = m_network.activate(inputs);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
      //  save(m_genome);
   //     exit(-1);
    }
    
    // Activation des sorties

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

    if(getTileFacing().flags.solid || getTileLeft().flags.solid|| getTileRight().flags.solid|| getTileBack().flags.solid)
    {
        wallHit++;
    }

    if(getTileOn().type == Type::CHECKPOINT)
    {
        numberOfCheckpoints++;
    }

    if (getTileOn().type == Type::FOOD)
    {
        end = true;
    }
    

    // Vérifier les répétitions d'actions
    if (direction == lastAction) {
        repeatCount++;
    } else {
        directionChanges++;
        repeatCount = 0; // Réinitialiser si la direction change
    }
    lastAction = direction;

    // Ajouter la position actuelle à l'ensemble
    visitedPositions.insert({static_cast<int>(m_gridPos.x), static_cast<int>(m_gridPos.y)});

    int wallProximityBefore = getWallProximityBeforeMove();

    Vec2i lastGridPos = m_gridPos;



    switch (direction) {
        case 0: move(RIGHT);  break;
        case 1: move(LEFT);  break;
        case 2: move(UP); break;
        case 3: move(DOWN); break;
        default: break;
    }

    int wallProximityAfter = getWallProximityBeforeMove();

    if (wallProximityAfter < wallProximityBefore) {
    goodWallAvoidanceMoves++;

    if (m_gridPos == lastGridPos) {
        stuckCount++;
    } else {
        stuckCount = 0; // Réinitialiser si la fourmi bouge
    }
    lastGridPos = m_gridPos;
}
}

bool simu::AntIA::isStuck()  {
    // Si la fourmi reste sur la même position pendant trop de ticks
    if (stuckCount >= 10) { // Par exemple, 10 ticks
        return true;
    }
    return false;
}

bool simu::AntIA::isIdle()
{
     // Si la fourmi répète trop souvent la même action
    if (repeatCount >= 15) { // Par exemple, 15 répétitions
        return true;
    }
    return false;
}

bool simu::AntIA::isCurrentPositionVisited()
{
    if (visitedPositions.find({static_cast<int>(m_gridPos.x), static_cast<int>(m_gridPos.y)}) != visitedPositions.end()) {
        return true;
    }
    return false;
}

int simu::AntIA::getWallProximityBeforeMove()
{
    return getTileFacing().flags.solid + 
                          getTileLeft().flags.solid + 
                          getTileRight().flags.solid+ 
                          getTileBack().flags.solid;
}

double simu::AntIA::getDistanceToWall(Vec2i dir)
{
    int distance = 0;
    Vec2i pos = m_gridPos;

    while (!getWorld().getGrid().getTile(pos).flags.solid) {
       
        pos += dir;
    }

    return pos.manhattan(pos);
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