#include "world.h"
#include "ant.h"

#include "external/ui/imgui.h"
#include "external/ui/rlImGui.h"

using namespace simu;

class Scene: public WorldListener
{
    std::vector<std::weak_ptr<AntIA>> ants;

    public:
        void onInit() override
        {
            Grid& grid = getWorld().getGrid();
            grid.fromImage("maze.png");
            getWorld().centerCamera();
                        
            ants = getWorld().spawnEntities<AntIA>(1000, Vec2i{89, 161});
            getWorld().spawnEntity<DemoAnt>(getWorld().gridToWorld(Vec2i{89, 161}));
        };

        void onDrawUI() override 
        {
            ImGui::Begin("Labyrinth settings");
            ImGui::End();
        };

        void onDraw() override {};
        void onUnload() override {};
        void onUpdate() override {};
};

int main(void)
{   
    SetTraceLogLevel(LOG_DEBUG);

    simu::World& world = simu::getWorld(); 
    world.setListener(std::make_shared<Scene>());
    return world.run(800, 800, "ants-simulation");
}