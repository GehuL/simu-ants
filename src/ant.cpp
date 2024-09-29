#include "ant.h"

#include "raylib.h"

#include "world.h"

using namespace simu;

Ant::Ant() : Entity("ant"), m_life(100.f)
{
}

void Ant::update()
{
    if(m_life >= 0)
        m_life -= 0.1f;
}

void Ant::draw() 
{
    DrawRectangle(this->m_posX, this->m_posY, 10, 10, BROWN);
}

void simu::Ant::eat()
{
    World& world = World::world;

    Tile tile = world.getGrid().getTile(m_posX, m_posY);
    
    if(tile.type == Type::FOOD)
    {
       if(m_life < 100.f)
            m_life += 10.f;

        world.getGrid().setTile(AIR, m_posX, m_posY);
    }
}
