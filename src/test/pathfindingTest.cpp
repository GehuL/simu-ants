#include "world.h"
#include "ant.h"

using namespace simu;

class Scene: public WorldListener
{
    public:
        void onInit() override
        {
            Grid& grid = getWorld().getGrid();
            grid.fromImage("maze.png");
            getWorld().centerCamera();
            
            grid.pathDistance(Vec2i{89, 161}, Vec2i{2, 2});
        };

        void onUnload() override
        {

        };
        
        void onUpdate() override
        {

        };
};

int main(void)
{   
    simu::World& world = simu::getWorld(); 
    world.setListener(std::make_shared<Scene>());
    return world.run(800, 800, "ants-simulation");
}