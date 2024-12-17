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
            
           /* double startTime = GetTime();
            auto path = grid.findPath(Vec2i{89, 161}, Vec2i{6, 10});

            double deltaTime = (GetTime() - startTime) * 1000.f;
            TraceLog(LOG_DEBUG, "A*: %.1lf ms", deltaTime);
        
            for(Vec2i tile : path)
                grid.setTile(FOOD, tile.x, tile.y);*/
        };

        void onDraw() override
        {
            Grid& grid = getWorld().getGrid();
            auto path = grid.findPath(Vec2i{89, 161}, getWorld().mouseToGridCoord());

            const int tileSize = grid.getTileSize();
            for(Vec2i tile : path)
            {
                DrawRectangle(tile.x * tileSize, tile.y * tileSize, tileSize, tileSize, RED);
            }
        }

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