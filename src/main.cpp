#include "world.h"
#include "ant.h"

int main(void)
{   
    simu::getWorld().spawnEntity<simu::Ant>();
    return simu::getWorld().run(800, 800, "ants-simulation");
}