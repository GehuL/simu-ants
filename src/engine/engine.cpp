#include "engine.h"
#include "utils.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "../external/ui/imgui.h"
#include "../external/ui/rlImGui.h"
#include <chrono>

#define MIN(a, b) a > b ? b : a

using namespace simu;

Engine::Engine() : m_tickPeriod(1.0/100.0), m_framePeriod(1.0/60.0)
{
    m_camera.zoom = 1.f;
}

int Engine::run(int screenWidth, int screenHeight, std::string title)
{
    double lastUpdateTime = 0;
    double lastDrawTime = 0;
    double lastGUITime = 0;

    double lag = 0.0;

    int tickCounter = 0;
    int frameCounter = 0;

    InitWindow(screenWidth, screenHeight, title.c_str());
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    
    m_renderer = LoadRenderTexture(screenWidth, screenHeight);
    m_gui_renderer = LoadRenderTexture(screenWidth, screenHeight);

    rlImGuiSetup(false);
    // ImGui::Spectrum::LoadStyleSynth();

    init();

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        m_profiler.begin("loop");

        double start = GetTime();

        if(!m_pause)
        {
            // Update logic
            lag += start - lastUpdateTime;
            lastUpdateTime = GetTime();

            if(lag / m_tickPeriod >= 100) // Evite de faire cramer le PC de M. Kirgisov
                lag = m_tickPeriod; 

            while(lag >= m_tickPeriod)
            {
                m_profiler.end("tps");
                m_profiler.begin<Profiler::UNSCOPED>("tps");

                m_profiler.begin("tick");
                updateTick();
                m_profiler.end();

                lag -= m_tickPeriod;
                tickCounter++;
                
                if(m_noDelay) 
                    break;
            }
        }

        // lastDrawTime += lag;
        double drawDelta = GetTime() - lastDrawTime;

        double uiDelta = GetTime() - lastGUITime;

        bool b_drawAll = drawDelta >= m_framePeriod;
        bool b_updateUI = uiDelta >= 1.0 / UI_FRAME_RATE; // Keep ui at a consistant frame rate

        // Draw frame OR update UI
        if(b_updateUI || b_drawAll)
        {
            m_profiler.begin("frame");

            BeginDrawing();

            if(b_drawAll)
            {
                m_profiler.end("fps");
                m_profiler.begin<Profiler::UNSCOPED>("fps");
                
                double now = GetTime();
                lastDrawTime = now;
                drawAll();
                frameCounter++;
            }

            // Draw frame
            DrawTexturePro(m_renderer.texture, {0, 0, (float)m_renderer.texture.width, -(float)m_renderer.texture.height}, {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, {0, 0}, 0, WHITE);
            
            // Draw UI at 30 FPS
            if(b_updateUI) 
            {
                m_profiler.end("ui");
                m_profiler.begin<Profiler::UNSCOPED>("ui");
                
                m_profiler.begin("ui_period");
                lastGUITime = GetTime();
                updateUI();
                PollInputEvents();
                m_profiler.end();
            }

            // Draw UI            
            DrawTexturePro(m_gui_renderer.texture, {0, 0, (float)m_gui_renderer.texture.width, -(float)m_gui_renderer.texture.height}, {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, {0, 0}, 0, WHITE);

            EndDrawing();

#ifdef SUPPORT_CUSTOM_FRAME_CONTROL
    SwapScreenBuffer();
#endif
            m_profiler.end();

        }

        double delta = GetTime() - start;

        // Différence de temps libre entre update et draw (drawDelta inclue le temps de l'affichage de l'UI)
        double waitTime = 0.0;
        if(m_pause)
            waitTime = m_framePeriod - delta;
        else
            waitTime = MIN(m_framePeriod - delta, m_tickPeriod - delta);
       
        if(waitTime >= 0.0) // Il reste du temps pour mettre en pause 
        {
            // Désactiver le waitTime permet d'augmenter la priorité du processus
            m_profiler.begin("idle");
            WaitTime(waitTime);
            m_profiler.end();
        }
        m_profiler.end();
    }
    unload();
    rlImGuiShutdown();

    CloseWindow();
    return 0;
}

void Engine::setFPS(float fps)
{
    if(fps <= 0)
        fps = 1;

    this->m_framePeriod = 1.0 / static_cast<double>(fps);
}

void Engine::setTPS(float tps)
{
    // Si fps = 0, m_tickPeriod = infinite
    if(tps <= 0)
    {
        setPause(true);
        tps = 1;
    }else
    {
        setPause(false);
    }
    this->m_tickPeriod = 1.0 / static_cast<double>(tps);
}

void Engine::drawAll()
{
    BeginTextureMode(m_renderer);

        ClearBackground(WHITE);

        BeginMode2D(m_camera);
            drawFrame();
        EndMode2D();

    EndTextureMode();
}

void Engine::updateTick()
{
}

void Engine::setPause(bool pause)
{
    this->m_pause = pause;
}

void Engine::drawFrame()
{
}

void Engine::drawUI()
{
}

void Engine::updateUI()
{
    if(IsWindowResized())
    {
        UnloadRenderTexture(m_renderer);
        UnloadRenderTexture(m_gui_renderer);

        m_renderer = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        m_gui_renderer = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }

    rlImGuiBegin();

    BeginTextureMode(m_gui_renderer);

    ClearBackground((Color){255, 255, 255, 0});
    // Controle de la simulation
    ImGui::SetNextWindowBgAlpha(0.5);
    ImGui::Begin("Engine", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);

    int framePerSecond = getFPS();
    if(ImGui::SliderInt("FPS", &framePerSecond, 1, 300))
        setFPS(framePerSecond);

    int tickPerSecond = getTPS();
    if(ImGui::SliderInt("TPS", &tickPerSecond, 0, 1000))
        setTPS(tickPerSecond);

    ImGui::Checkbox("Pause", &m_pause); // { setPause(m_pause); }
    ImGui::SameLine(); if(ImGui::Button("Single step")) { updateTick(); }
    ImGui::SameLine(); if(ImGui::Button("No limit")) { setTPS(999999999); }
    
    ImGui::End();

    // Affichage profiling
    ImGui::SetNextWindowBgAlpha(0.8f);
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    if(ImGui::Begin("Overlay performance", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        ImGui::Text("Profiler");
        ImGui::Separator();

        ImGui::Text("FPS: %d | Load: %.2lf/%.2lf ms (%.2lf%%)", 
        static_cast<int>(m_profiler["fps"]->getFrequency()), m_profiler["frame"]->calculAverage().count() * 1000.0, m_framePeriod * 1000.0,
        m_profiler["frame"]->calculAverage().count()/m_framePeriod * 100);

        ImGui::Text("TPS: %d | Load: %.2lf/%.2lf ms (%.2lf%%)",
        static_cast<int>(m_profiler["tps"]->getFrequency()), m_profiler["tick"]->calculAverage().count() * 1000.0, m_tickPeriod * 1000.0,
        m_profiler["tick"]->calculAverage().count()/m_tickPeriod * 100);

        ImGui::Text("UI: %d | Load: %.2lf/%.2lf ms (%.2lf%%)",
        static_cast<int>(m_profiler["ui"]->getFrequency()), m_profiler["ui_period"]->calculAverage().count() * 1000.0, 1.0 / UI_FRAME_RATE * 1000.0,
        m_profiler["ui_period"]->calculAverage().count() *  UI_FRAME_RATE * 100);

        ImGui::Text("Total: %.2lf ms | Idle: %.2lf ms", m_profiler["loop"]->calculAverage().count() * 1000.0, m_profiler["idle"]->calculAverage().count() * 1000.0);
    }
    ImGui::End();

    ImGui::ShowDemoWindow();
    drawUI();

    rlImGuiEnd();

    EndTextureMode();
}
