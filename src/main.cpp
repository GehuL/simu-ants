#include "world.h"

#include "ant.h"

#include <iostream>

int main(void)
{   
    auto& world = simu::getWorld();
   
    auto ant = world.spawnEntity<simu::Ant>();

    std::cout << *ant.lock() << std::endl;

    if(world.exist(0))
    {
        auto en = world.getEntity<simu::Ant>(0);
        std::cout << *en.lock() << std::endl;
    }

    world.spawnEntity<simu::Ant>();
    std::cout << *world.getEntity<simu::Entity>(1).lock() << std::endl;

    return world.run(800, 800, "ants-simulation");
}