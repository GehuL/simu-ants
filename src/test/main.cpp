#include "../engine/world.h"

#include "../external/ui/imgui.h"
#include "../external/ui/rlImGui.h"
#include "../external/json.hpp"

#include "../simulation/demo.h"
#include "../simulation/labyrinth.h"

#include "utils.h"



int main(void)
{   
    SetTraceLogLevel(LOG_DEBUG);
    
    simu::World& world = simu::getWorld(); 
    world.registerLevel<simu::Demo>("Démonstration 1");
    world.registerLevel<simu::Demo2>("Démonstration 2");
    world.registerLevel<simu::Labyrinth>("Labyrinthe");
    return world.run(1600, 800, "ants-simulation");
}