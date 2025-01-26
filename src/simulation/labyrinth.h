#ifndef __LABYRINTH__H
#define __LABYRINTH__H

#include "../engine/world.h"

namespace simu
{
    using namespace simu;
    using json = nlohmann::json;

    class Labyrinth: public Level
    {
        public:
            void onInit() override
            {
                Grid& grid = getWorld().getGrid();
                grid.fromImage("maze.png");
                getWorld().spawnEntities<AntIA>(1000, Vec2i{89, 161});
            };

            const std::string getDescription() const override { return "Simulation d'apprendissage de résolution de labyrinthe."; };
    };

}

#endif 