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

            // Test get Entities
            auto entities = getWorld().getEntities();
            for(std::weak_ptr<Entity> en : entities)
                std::cout << en.lock()->getType() << std::endl;
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