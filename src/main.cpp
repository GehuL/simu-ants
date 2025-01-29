#include "engine/world.h"

#include "simulation/Demo.h"
#include "simulation/laborer.h"
#include "simulation/maze.h"
#include "simulation/mazeSpe.h"
#include "simulation/minimazeSpe.h"
#include "simulation/minimaze.h"
#include "simulation/road.h"


int main(void) {
    SetTraceLogLevel(LOG_DEBUG);
    simu::World &world = simu::getWorld();
    world.registerLevel<Demo>("Demo");
    world.registerLevel<Laborer>("Laborer");
    world.registerLevel<MazeCheck>("MazeCheck");
    world.registerLevel<Road>("Road");
    world.registerLevel<MazeCheckSpe>("MazeCheckSpe");
    world.registerLevel<MiniMaze>("MiniMaze");
    world.registerLevel<MiniMazeSpe>("MiniMazeSpe");
    return world.run(800, 800, "Ants Labyrinth Simulation");;
}