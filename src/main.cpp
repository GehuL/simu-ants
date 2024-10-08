#include "world.h"
#include "ant.h"

int main(void)
{   
    simu::World& world = simu::getWorld(); 
    world.init();
    return world.run(800, 800, "ants-simulation");
}