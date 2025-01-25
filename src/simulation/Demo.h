#ifndef __DEMO__H
#define __DEMO__H

#include "../engine/world.h"

namespace simu
{
    using namespace simu;
    using json = nlohmann::json;

    class Demo: public Level
    {
        public:
            void onInit() override
            {
                Grid& grid = getWorld().getGrid();
                grid.fromImage("maze.png");
                getWorld().spawnEntities<DemoAnt>(1000, Vec2i{89, 161});
            };

            /*void onDrawUI() override 
            {
                ImGui::Begin("Démo");
                ImGui::End();
            };

            void onLoad(json j) override
            {
                TRACELOG(LOG_INFO, "Loaded !");
            };

            void onSave(json j) override
            {
                TRACELOG(LOG_INFO, "Saved !");
            };*/

            const std::string getDescription() const override { return "Scene de démonstration du moteur. Pas de script d'apprentissage."; };
    };

    class Demo2: public Level
    {
        void onInit() override
        {
            Grid& grid = getWorld().getGrid();
            grid.fromImage("maze.png");
                        
            getWorld().spawnEntities<AntIA>(1000, Vec2i{89, 161});
        };

        const std::string getDescription() const override { return "Scene de démonstration du moteur. Pas de script d'apprentissage."; };
    };
}

#endif // __DEMO__H