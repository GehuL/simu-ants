#include "../engine/world.h"
#include "../NEAT/Population.h"
#include "../NEAT/evolution.h"

using namespace simu;

class Evolution : public WorldListener
{
    public:
        Evolution(int reproductionPeriod) : m_reproductionPeriod(reproductionPeriod) {};

        void onInit() override
        {
            getWorld().getGrid().fromImage("rsc/maze.png");
            getWorld().spawnEntities<DemoAnt>(10);
        };

        void onUpdate() override
        {
        };

        void onUnload() override
        {

        };

    private:
        int m_reproductionTick;
        int m_reproductionPeriod;
};

int main()
{
    RNG rng;
    NeatConfig config {.num_inputs=3, .num_outputs=1};

    World& world = getWorld();
    world.setListener(std::make_shared<Evolution>(rng, config));

    return world.run(800, 800, "Maze evolution");
}