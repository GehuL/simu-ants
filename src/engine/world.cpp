#include "world.h"
#include <exception>


#include "utils.h"
#include "ant.h"

#include "raygui.h"

#include "../external/json.hpp"
#include "../external/ui/imgui.h"
#include "../external/ui/rlImGui.h"
#include "../external/ui/imgui_internal.h"


using namespace simu;
using json = nlohmann::json;

World World::world;

World::World() : m_entity_cnt(0), m_grid(5)
{

}

void World::init()
{
    Engine::init();

    m_seed = GetRandomValue(0, std::numeric_limits<int>::max());
    SetRandomSeed(m_seed);

    clearEntities();

    if(m_level)
        m_level.get()->onInit();
}

void World::unload()
{
    if(m_level)
        m_level.get()->onUnload();
    m_grid.unload();
}

Vector2i World::mouseToGridCoord() const
{
    Vector2f pos = GetScreenToWorld2D(GetMousePosition(), m_camera);
    return m_grid.toTileCoord(pos.x, pos.y);
}

Vec2f World::gridToWorld(Vector2i pos) const
{
    return Vec2f{pos * m_grid.getTileSize()};
}

void World::centerCamera()
{
    float offset = (GetScreenWidth() - m_grid.getGridWidth() * m_grid.getTileSize()) / 2.f;
    m_camera.offset = (Vector2){offset, offset};
}

Tile World::getSelectedTile() const
{
    return m_cursorTiles[m_cursorTileIndex];
}

void World::save(const std::string& filename)
{
    //TRACELOG(LOG_INFO, "Saving simulation..");
 
    try
    {
        json j;

        for(auto& en : m_entities)
        {
            Entity& a = *en.get();
            json ja;
            a.save(ja);
            j["entities"] += ja;
        }

        j["grid"] = m_grid;
        j["seed"] = m_seed;

        if(m_level)
            m_level.get()->onSave(j);

        auto file = std::ofstream(filename, std::ios_base::out);
        file << j;
        file.close();

        //TRACELOG(LOG_INFO, "File saved to %s", filename.c_str());
    }catch(const json::exception& e)
    {
        //TRACELOG(LOG_ERROR, "Erreur de chargement du fichier %s: %s", filename, e.what());
    } 
}

void World::load(const std::string& filename)
{
    //TRACELOG(LOG_INFO, "Loading file %s", filename.c_str());
    
    try
    {
        auto file = std::ifstream(filename, std::ios_base::in);
        
        json j = json::parse(file);
        
        // Lecture des entités
        auto entities_json = j.at("entities");
        std::vector<std::shared_ptr<Entity>> entities_tmp;

        if(entities_json.is_array())
        {
            entities_tmp.resize(entities_json.size());

            // Instantie la bonne class en fonction du type json
            for(size_t i = 0; i < entities_json.size(); i++)
            {
                std::string typestr = entities_json[i]["type"];
                entities_t t = entityFactory(typestr);
                std::visit([&](auto& e) mutable {
                    using EntityType = std::decay_t<decltype(e)>;
                    auto en = std::make_shared<EntityType>(m_entity_cnt++);
                    auto en_j = entities_json[i];
                    en.get()->load(en_j);
                    entities_tmp[i] = en;
                }, t);
            }
        }

        // Lecture de la grille
        j.at("grid").get_to(m_grid);
        
        if(j.find("seed") != j.end())
        {
            m_seed = j.at("seed");
            SetRandomSeed(m_seed);
        }

        // Si on arrive ici c'est qu'il n'y a pas eu d'erreurs
        m_entities.clear();
        m_entities = std::move(entities_tmp); // On peut altérer la partie
       
        if(m_level)
            m_level.get()->onLoad(j);

        file.close();

        //TRACELOG(LOG_INFO, "Loaded !");
    } catch(const json::exception& e)
    {
        //TRACELOG(LOG_ERROR, "Erreur de chargement du fichier %s: %s", filename, e.what());
    } catch(const std::runtime_error& e)
    {
        //TRACELOG(LOG_ERROR, "Erreur de chargement du fichier %s: %s", filename, e.what());
    }
}

void World::handleMouse()
{
    if(ImGui::GetIO().WantCaptureMouse)
        return;

    // TODO: Interpoler les points pour tracer des lignes
    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), m_camera);

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        auto entity = getEntityAt(mousePos);

        if(!entity.expired()) 
        {
            if(!m_selected_en.expired() && m_selected_en.lock()->getId() == entity.lock()->getId())
            {
                m_selected_en.reset(); // Déselectionne la fourmis
            }else
            {
                m_selected_en = entity; // Sélectionne 
                m_focus_en_gui = true;
            }
        }
    }else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) 
    {
        auto entity = getEntityAt(mousePos);
        if(entity.expired())
         m_grid.setTile(getSelectedTile(), mousePos.x, mousePos.y);
    }else if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) // REMOVE TILE
    {
        m_grid.setTile(AIR, mousePos.x, mousePos.y);
    }
    
    // Changer de type de tuile
    if(IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
    {
        m_cursorTileIndex++;
        m_cursorTileIndex %= m_cursorTiles.size();
    }

    if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
    {
        m_camera.offset.x += GetMouseDelta().x;
        m_camera.offset.y += GetMouseDelta().y;
    }

    float zoomIncrement = GetMouseWheelMove() * 0.1f;
    if (zoomIncrement != 0.0f) 
    {
        m_camera.zoom += zoomIncrement;
       
        // Empêcher un zoom négatif ou trop petit
        if (m_camera.zoom < 0.1f) m_camera.zoom = 0.1f; 

        // Recalculer la position de la caméra pour garder la position de la souris au même endroit
        Vector2 newWorldMousePosition = GetScreenToWorld2D(GetMousePosition(), m_camera);
        m_camera.target.x -= (newWorldMousePosition.x - mousePos.x);
        m_camera.target.y -= (newWorldMousePosition.y - mousePos.y);
    }
}

void World::handleKeyboard()
{
    if(ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow | ImGuiFocusedFlags_AnyWindow))
        return;

    constexpr float camera_speed = 5.0f; 

    if (IsKeyDown(KEY_DOWN)) m_camera.offset.y -= camera_speed;
    if (IsKeyDown(KEY_UP)) m_camera.offset.y += camera_speed;
    if (IsKeyDown(KEY_RIGHT)) m_camera.offset.x -= camera_speed;
    if (IsKeyDown(KEY_LEFT)) m_camera.offset.x += camera_speed;

    if(IsKeyPressed(KEY_ENTER)) init();

    if(IsKeyPressed(KEY_P)) setPause(!isPaused());

    if(IsKeyPressed(KEY_S))
        save("simu-save.json");

    if(IsKeyPressed(KEY_R))
        load("simu-save.json");
    
    if(IsKeyPressed(KEY_SPACE))
    {
        if(isPaused())
        {
            updateTick();
        }
    }
}

void World::drawFrame()
{
    m_grid.draw();

    for(auto& en : m_entities)
    {
        en->draw();
    }

    if(m_level)
        m_level.get()->onDraw();
}

void World::drawUI()
{
    handleKeyboard();
    handleMouse();

    drawEntityInfo();

    // Coordonnées de la souris
    Vector2i mousePos = mouseToGridCoord();
    DrawText(TextFormat("(X: %d|Y: %d)", mousePos.x, mousePos.y), 5, GetScreenHeight() - 20, 16, GRAY);
    
    // Type de tuile sélectionné
    DrawText("Tile: ", 100, GetScreenHeight() - 20, 16, GRAY);
    DrawRectangle(135, GetScreenHeight() - 17, 10, 10, getSelectedTile().color);

    // Nombre d'entités
    DrawText(TextFormat("Entity: %d", m_entities.size()), 0, 100, 20, BLUE);

    ImGui::Begin("World");
    ImGui::Text("Seed: 0x%X", m_seed);
    
    // Sauvegarde et chargement
    static char saveFileName[128] = "simu-save.json";
    ImGui::InputText("##input_file", saveFileName, IM_ARRAYSIZE(saveFileName));
    ImGui::SameLine();
    if(ImGui::Button("Save")) save(saveFileName);

    static char loadFileName[128] = "simu-save.json";
    ImGui::InputText("##output_file", loadFileName, IM_ARRAYSIZE(loadFileName));
    ImGui::SameLine();
    if(ImGui::Button("Load")) load(loadFileName);


    if(ImGui::CollapsingHeader("Level"))
    {
        // Affichage de la liste des niveaux disponibles
        std::string selectedLevel = m_level ? m_level->getName() : "None";
        if(ImGui::BeginCombo("Levels", selectedLevel.c_str()))
        {
            for(auto& [name, level] : m_levels)
            {
                if(ImGui::Selectable(name.c_str(), selectedLevel == name))
                {
                    loadLevel(name);
                    break;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if(ImGui::Button("Restart Level")) init();

        if(m_level != nullptr)
        {
            // Level* level = dynamic_cast<Level*>(m_level.get());
            ImGui::SeparatorText("Description");
            ImGui::Text("%s", m_level->getDescription().substr(0, 256).c_str());
        }
    }

    if(ImGui::CollapsingHeader("Grid"))
    {
        ImGui::Text("Size: %dx%d", m_grid.getGridWidth(), m_grid.getGridWidth());
        static char filename[128] = "maze.png";
        ImGui::InputText("File name", filename, IM_ARRAYSIZE(filename));
        ImGui::SameLine();
        static std::runtime_error last_error("");
        if(ImGui::Button("Load grid"))
        {
            try
            {
                m_grid.fromImage(std::string(filename));
            }catch(std::runtime_error& error)
            {
                last_error = error;
                ImGui::OpenPopup("Error");
            }
        }
        
        if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(last_error.what());
            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }

    if(ImGui::CollapsingHeader("Entity"))
    {
        ImGui::Text("Entity count: %lld", m_entities.size());
    
        if(ImGui::Button("Remove all")) clearEntities();
        
        static char input[6] = ""; ImGui::InputText("##", input, 6, ImGuiInputTextFlags_CharsDecimal);
        ImGui::SameLine();
        if(ImGui::Button("Spawn DemoAnt"))
        {
            try
            {
                int number = std::stoi(input);
                if(number < 0 || number > 99999) throw std::runtime_error("Invalid number");
                auto entities = spawnEntities<DemoAnt>(number, Vec2f(0, 0));
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }

        if(ImGui::TreeNode("List"))
        {
            ImGuiListClipper clipper; // Evite d'itérer sur toutes les entités
            clipper.Begin(m_entities.size());

            if(m_focus_en_gui && !m_selected_en.expired())
            {
                const Entity* entity = m_selected_en.lock().get(); 
                clipper.IncludeItemByIndex(entity->getId());
            }

            while(clipper.Step()) 
            {
                for(int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                {
                    Entity* entity = m_entities[row_n].get();

                    ImGuiTreeNodeFlags nodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
                    if(!m_selected_en.expired() && (m_selected_en.lock()->getId() == entity->getId()))
                    {
                        nodeFlag |= ImGuiTreeNodeFlags_Selected;
                    }

                    if(ImGui::TreeNodeEx((void*) entity->getId(), nodeFlag, "%s Id:%ld", entity->getType(), entity->getId()))
                    {
                        if(ImGui::IsItemFocused()) 
                            m_selected_en = m_entities[row_n];
                        // if(ImGui::Selectable(nodeId)) m_selected_en = m_entities[row_n];
                        // if(ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::IsMouseClicked(ImGuiPopupFlags_MouseButtonLeft))

                        Vec2i gridPos = m_grid.toTileCoord(entity->getPos());
                        Vec2f globalPos = entity->getPos();

                        ImGui::Text("Grid pos: (%d, %d)", gridPos.x, gridPos.y);
                        ImGui::Text("Global pos: (%.1f, %.1f)", globalPos.x, globalPos.y);

                        ImGui::TreePop();
                    }

                    if(!m_selected_en.expired() && (m_selected_en.lock()->getId() == entity->getId()))
                    {
                        if(m_focus_en_gui)
                        {
                            m_focus_en_gui = false;
                            ImGui::ScrollToItem(ImGuiScrollFlags_KeepVisibleCenterY);
                        }
                    }
                }
            }
            ImGui::TreePop();
        }
    }

    ImGui::End();

    if(m_level)
        m_level.get()->onDrawUI();
}

void World::drawEntityInfo()
{
    if(!m_selected_en.expired())
    {
        const Entity* en = m_selected_en.lock().get();
       
        Vec2f enPos = en->getPos();
        Vec2f pos = GetWorldToScreen2D((Vector2){enPos.x, enPos.y}, m_camera);
       
        DrawRectangleLines(pos.x - 1, pos.y - 1, 6, 6, YELLOW);
       
        const char* text = TextFormat("Id: %d Type: %s", en->getId(), en->getType());
        const Color boxColor = (Color) {0x03, 0xd3, 0xfc, 0xA5};
        const int buttonHeight = 18;
        int boxeWidth =  MeasureText(text, GuiGetStyle(LABEL, TEXT_SIZE));

        Rectangle boxPos = {pos.x + 50, pos.y + 50, (float) boxeWidth + 16, 50};

        DrawRectangle(boxPos.x, boxPos.y, boxPos.width, boxPos.height, boxColor);
        DrawLineEx((Vector2) {pos.x, pos.y}, (Vector2) {boxPos.x, boxPos.y}, 4, boxColor);

        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0xffff);
        GuiLabel((Rectangle) {boxPos.x + 8, boxPos.y - 8, boxPos.width, boxPos.height}, text);
        
        if(typeid(*en) == typeid(AntIA))
        {
            Rectangle buttonBox = (Rectangle) {boxPos.x + 4, boxPos.y + 28, boxPos.width - 8, buttonHeight};
            
            if(GuiButton(buttonBox, "Open Genome")) m_showGenome = true;
          
            if(m_showGenome)
            {
                if(GuiWindowBox((Rectangle) {80, 80, GetScreenWidth() - 160, GetScreenHeight() - 160}, TextFormat("Génome N°%d", en->getId())))
                    m_showGenome = false;
            }
        }
    }
}

void World::updateTick()
{
    m_grid.update();

    for(size_t i = 0; i < m_entities.size(); i++)
    {
        auto en = m_entities[i];
        if(en != nullptr)
        {
            en->update();
        }
    }

    if(m_level)
        m_level.get()->onUpdate();
}

bool World::exist(unsigned long id) const
{
    if(m_entities.size() < 0)
        return false;

    return std::binary_search(m_entities.begin(), m_entities.end(), nullptr, [id](const std::shared_ptr<Entity>& v1, const std::shared_ptr<Entity>& v2) 
    {
        return v1 && v1->getId() < id;
    });
}

bool World::removeEntity(unsigned long id)
{
    if(m_entities.size() < 1)
        return false;

    auto it = std::lower_bound(m_entities.begin(), m_entities.end(), nullptr, [id](const auto& v1, const auto& v2) {
        return v1->getId() < id;
    });

    m_entities.erase(it);

    if(it != m_entities.end())
    {
       // if(id == (m_entity_cnt - 1)) // Source de bugs .. avoir plus tard
       //     m_entity_cnt--; // Maintient l'ordre tout en évitant la limite atteinte (casi impossible)

        return true;
    }
    return false;
}

std::weak_ptr<Entity> World::getEntityAt(Vec2f pos)
{
    auto en_it = std::find_if(m_entities.begin(), m_entities.end(), [&](const auto& en) {
        // Distance de manhattan (+ rapide que euclidien car lent pour beaucoup d'entité)
        return (en.get()->getPos() + Vec2f(2.5f, 2.5f)).manhattan(pos) < 5; 
    });

    return std::weak_ptr<Entity>(en_it == m_entities.end() ? nullptr : *en_it);
}

void World::clearEntities() 
{ 
    m_entities.clear();
    m_entity_cnt = 0;
}

void World::loadLevel(const std::string& name)
{
    if(m_levels.find(name) == m_levels.end())
        throw std::runtime_error("Le niveau " + name + " n'existe pas");
   
    // Unload le précédent
    if(m_level) { m_level.get()->onUnload(); }
    m_levels[name]();

    TraceLog(LOG_DEBUG, "Niveau %s chargé", name.c_str());
}