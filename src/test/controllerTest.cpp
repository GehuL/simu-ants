#include "../engine/world.h"
#include "../NEAT/Population.h"

using namespace simu;

class Evolution : public WorldListener
{
    public:
        Evolution(RNG& rng, NeatConfig config) : m_pop(config, rng) {};

        void onInit() override
        {
            getWorld().getGrid().fromImage("rsc/maze.png");

            for(auto en : m_pop.get_individuals())
            {
                auto ant = getWorld().spawnEntity<Ant>();
                AntController controller(ant, en.genome);
                m_controllers.push_back(controller);
            }
        };

        void onUpdate() override
        {
            for(auto& c : m_controllers)
                c.activate();
        };

        void onUnload() override
        {

        };

    private:
        Population m_pop;
        std::vector<AntController> m_controllers;
};

int main()
{
    RNG rng;
    NeatConfig config {.num_inputs=3, .num_outputs=1};

    World& world = getWorld();
    world.setListener(std::make_shared<Evolution>(rng, config));

    return world.run(800, 800, "Maze evolution");
}