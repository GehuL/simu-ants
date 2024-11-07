#include "world.h"
#include "ant.h"

using namespace simu;

class Scene: public WorldListener
{
    public:
        void onInit() override
        {
            getWorld().getGrid().fromImage("maze.png");
            getWorld().spawnEntities<DemoAnt>(10, getWorld().gridCoordToWorld(Vector2i{89, 161}));
            getWorld().centerCamera();
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