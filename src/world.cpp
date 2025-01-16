#include "world.h"
#include <exception>

#include "utils.h"
#include "ant.h"
#include "external/json.hpp"

#include "raygui.h"

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

    TRACELOG(LOG_INFO, "seed: %d", m_seed);
    m_entities.clear();

    if(m_listener)
        m_listener.get()->onInit();
}

void World::unload()
{
    if(m_listener)
        m_listener.get()->onUnload();
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
    TRACELOG(LOG_INFO, "Saving simulation..");
 
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

        auto file = std::ofstream(filename, std::ios_base::out);
        file << j;
        file.close();

        TRACELOG(LOG_INFO, "File saved to %s", filename.c_str());
    }catch(const json::exception& e)
    {
        TRACELOG(LOG_ERROR, "Erreur de chargement du fichier %s: %s", filename, e.what());
    } 
}

void World::load(const std::string& filename)
{
    TRACELOG(LOG_INFO, "Loading file %s", filename.c_str());
    
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
                std::visit([&](auto e) mutable {
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
       
        file.close();

        TRACELOG(LOG_INFO, "Loaded !");
    } catch(const json::exception& e)
    {
        TRACELOG(LOG_ERROR, "Erreur de chargement du fichier %s: %s", filename, e.what());
    } catch(const std::runtime_error& e)
    {
        TRACELOG(LOG_ERROR, "Erreur de chargement du fichier %s: %s", filename, e.what());
    }
}

void World::handleMouse()
{
    // TODO: Interpoler les points pour tracer des lignes
    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), m_camera);

    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) 
    {
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

    if(IsKeyPressed(KEY_Q)) // Selection entité
    {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), m_camera);

        // Test si une entité est cliqué
        auto en_it = std::find_if(m_entities.begin(), m_entities.end(), [&](const auto& en) {
            // Distance de manhattan (+ rapide que euclidien car lent pour beaucoup d'entité)
            return (en.get()->getPos() + Vec2f(2.5f, 2.5f)).manhattan(mousePos) < 5; 
        });

        // C'est une tuile
        if(en_it != m_entities.end()) 
        {
            if(!m_selected_en.expired() && m_selected_en.lock()->getId() == en_it->get()->getId())
            {
                m_selected_en.reset(); // Déselectionne la fourmis
            }else
            {
                m_selected_en = *en_it; // Sélectionne 
            }
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

    if(m_listener)
        m_listener.get()->onDraw();

    drawEntityInfo();
}

void World::drawUI()
{
    handleKeyboard();
    handleMouse();

    // Coordonnées de la souris
    Vector2i mousePos = mouseToGridCoord();
    DrawText(TextFormat("(X: %d|Y: %d)", mousePos.x, mousePos.y), 5, GetScreenHeight() - 20, 16, GRAY);
    
    // Type de tuile sélectionné
    DrawText("Tile: ", 100, GetScreenHeight() - 20, 16, GRAY);
    DrawRectangle(135, GetScreenHeight() - 17, 10, 10, getSelectedTile().color);

    // Nombre d'entités
    DrawText(TextFormat("Entity: %d", m_entities.size()), 0, 100, 20, BLUE);
}

void World::drawEntityInfo()
{
    if(!m_selected_en.expired())
    {
        const Entity* en = m_selected_en.lock().get();
       
        Vec2f pos = en->getPos();
       
        DrawRectangleLines(pos.x - 1, pos.y - 1, 6, 6, YELLOW);
       
        const char* text = TextFormat("Id: %d Type: %s", en->getId(), en->getType());
        const Color boxColor = (Color) {0x03, 0xd3, 0xfc, 0x55};
        const int buttonHeight = 18;
        int boxeWidth =  MeasureText(text, GuiGetStyle(LABEL, TEXT_SIZE));

        Rectangle boxPos = {pos.x + 50, pos.y + 50, (float) boxeWidth + 16, 50};

        if(typeid(*en) == typeid(AntIA))
            boxPos.width += buttonHeight + 5;

        DrawRectangle(boxPos.x, boxPos.y, boxPos.width, boxPos.height, boxColor);
        DrawLineEx((Vector2) {pos.x, pos.y}, (Vector2) {boxPos.x, boxPos.y}, 4, boxColor);

        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0xffff);
        GuiLabel((Rectangle) {boxPos.x + 8, boxPos.y - 8, boxPos.width, boxPos.height}, text);
        
        if(typeid(*en) == typeid(AntIA))
        {
            Rectangle buttonBox = (Rectangle) {boxPos.x + 2, boxPos.y + 28, boxPos.width - 8, buttonHeight};
            Color btnColor = GREEN;            
            if(CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), m_camera), buttonBox))
            {
                btnColor = DARKGREEN;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
            }
            
            DrawRectangleRec(buttonBox, btnColor);
            DrawRectangleLinesEx(buttonBox, 2.5, GRAY);
            DrawText("Open Genome", boxPos.x + 8, boxPos.y + 30, 12, BLUE);
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

    if(m_listener)
        m_listener.get()->onUpdate();
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

    return it != m_entities.end();
}
