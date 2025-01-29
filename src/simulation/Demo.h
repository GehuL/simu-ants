#ifndef __DEMO__H
#define __DEMO__H

#include "../engine/world.h"
#include "../external/ui/imgui.h"

namespace simu
{
    using namespace simu;
    using json = nlohmann::json;

    class Demo: public Level
    {
        public:
            Demo(std::string name): Level(name) {};

            void onInit() override
            {
                Grid& grid = getWorld().getGrid();
                try
                {
                    grid.fromImage("rsc/maze.png");
                }catch(const std::exception& e)
                {
                    TraceLog(LOG_ERROR, "Impossible de charger l'image du labyrinthe: %s", e.what());
                }
                getWorld().spawnEntities<DemoAnt>(1000, Vec2i{89, 161});
            };

            void onUnload() override
            {
                TraceLog(LOG_INFO, "Unloaded !");
            };

            void onDrawUI() override 
            {
                ImGui::Begin("Démo");
                ImGui::End();
            };

            void onLoad(json j) override
            {
                TraceLog(LOG_INFO, "Loaded !");
            };

            void onSave(json j) override
            {
                TraceLog(LOG_INFO, "Saved !");
            };

            const std::string getDescription() const override { return "Scene de démonstration du moteur. Pas de script d'apprentissage."; };
    };

    class Demo2: public Level
    {
        public:
            Demo2(std::string name): Level(name) {};

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