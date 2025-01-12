#include "world.h"
#include "ant.h"

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
            
            TraceLog(LOG_DEBUG, "Size of Type: %d  Size of Tiles: %d", sizeof(Type), sizeof(Tile));
            
           /* double startTime = GetTime();
            auto path = grid.findPath(Vec2i{89, 161}, Vec2i{6, 10});

            double deltaTime = (GetTime() - startTime) * 1000.f;
            TraceLog(LOG_DEBUG, "A*: %.1lf ms", deltaTime);
        
            for(Vec2i tile : path)
                grid.setTile(FOOD, tile.x, tile.y);*/

            ants = getWorld().spawnEntities<AntIA>(10, Vec2i{89, 161});
        };

        void onDraw() override
        {
            if(IsKeyPressed(KEY_G))
            {
                Grid& grid = getWorld().getGrid();

                for(const auto& ant: ants)
                {
                    if(ant.expired())
                        continue;

                    Vec2i antPos = grid.toTileCoord((Vec2f)(ant.lock()->getPos()));

                    auto path = grid.findPath(antPos, getWorld().mouseToGridCoord());

                    const int tileSize = grid.getTileSize();

                    for(Vec2i tile : path)
                    {
                        DrawRectangle(tile.x * tileSize, tile.y * tileSize, tileSize, tileSize, RED);
                    }
                }
            }

            if(IsKeyPressed(KEY_SPACE))
            {
                // getWorld().clearEntities();
                //  for(auto& en : getWorld().getEntities())
                // {
                //     getWorld().removeEntity(en.lock()->getId());
                // }
                getWorld().removeEntities(ants.begin(), ants.end());
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