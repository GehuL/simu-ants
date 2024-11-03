#include "engine/world.h"
#include "engine/ant.h"

int main(void)
{   
    simu::World& world = simu::getWorld(); 
    return world.run(800, 800, "ants-simulation");
}