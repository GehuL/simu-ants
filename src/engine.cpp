#include "engine.h"
#include "utils.h"
#include "raygui.h"

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
    double lastProfilerTime = 0;
    double lastGUITime = 0;

    int tickCounter = 0;
    int frameCounter = 0;

    InitWindow(screenWidth, screenHeight, title.c_str());
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    
    m_renderer = LoadRenderTexture(screenWidth, screenHeight);
    m_gui_renderer = LoadRenderTexture(screenWidth, screenHeight);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update logic
        double updateDelta = 0.0;
        if(!m_pause)
        {
            updateDelta = GetTime() - lastUpdateTime;
            if(updateDelta >= m_tickPeriod)
            {
                double now = GetTime();
                lastUpdateTime = now;
                updateTick();
                tickCounter++;
            }
        }

        double drawDelta = GetTime() - lastDrawTime;
        double uiDelta = GetTime() - lastGUITime;

        bool b_drawAll = drawDelta >= m_framePeriod;
        bool b_updateUI = uiDelta > 1.0 / 30.0;

        // Draw frame OR update UI
        if(b_updateUI || b_drawAll)
        {
            BeginDrawing();

            if(b_drawAll)
            {
                double updateDelta2 = GetTime() - lastUpdateTime;
                if(updateDelta2 >= m_tickPeriod && !m_noDelay) // Pas le temps pour dessiner, priorité sur update
                {
                    frameCounter--;
                    TRACELOG(LOG_INFO, "Frame skipped !");
                }else
                {
                    double now = GetTime();
                    lastDrawTime = now;
                    drawAll();
                }
            }

            // Draw frame
            DrawTexturePro(m_renderer.texture, {0, 0, (float)m_renderer.texture.width, -(float)m_renderer.texture.height}, {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, {0, 0}, 0, WHITE);
            
            // Draw UI at 30 FPS
            if(b_updateUI) 
            {
                lastGUITime = GetTime();
                drawUI();
                PollInputEvents();
            }

            // Draw UI            
            DrawTexturePro(m_renderer.texture, {0, 0, (float)m_renderer.texture.width, -(float)m_renderer.texture.height}, {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, {0, 0}, 0, WHITE);

            EndDrawing();

#ifdef SUPPORT_CUSTOM_FRAME_CONTROL
    SwapScreenBuffer();
#endif
        }

        // Profiling update
        double now = GetTime();
        double profilerDelta = now - lastProfilerTime;
        if(profilerDelta > 1.0)
        {
            lastProfilerTime = now; // pas besoin d'être précis pour le profiling
            
            m_lastFrameCounter = frameCounter;
            frameCounter = 0;

            m_lastTickCounter = tickCounter;
            tickCounter = 0;
        }
    
        // Différence de temps libre entre update et draw
        double waitTime = MIN(m_framePeriod - drawDelta, m_tickPeriod - updateDelta);
        if(!m_noDelay && waitTime >= 0.0) // Il reste du temps pour mettre en pause  
        {
            // Désactiver le waitTime permet d'augmenter la priorité du processus 
            WaitTime(waitTime);
        }
    }
    CloseWindow();
    return 0;
}

void Engine::setFPS(int fps)
{
    if(fps <= 0)
        fps = 1;

    this->m_framePeriod = 1.0 / static_cast<double>(fps);
}

void Engine::setTPS(int tps)
{
    // Si fps = 0, m_tickPeriod = infinite
    m_noDelay = false;
    if(tps >= 1000)
    {
        m_noDelay = true;
        tps = 999999999;
    }else if(tps <= 0)
    {
        m_pause = true;
        tps = 0;
    }else
    {
        m_pause = false;
    }
    this->m_tickPeriod = 1.0 / static_cast<double>(tps);
}

void Engine::drawAll()
{
    BeginTextureMode(m_renderer);

        ClearBackground(RAYWHITE);

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
    DrawText(TextFormat("%d FPS\n%d TPS", m_lastFrameCounter, m_lastTickCounter), 5, 0, 20, GREEN);

    if(m_pause)
    {
        const char* pause_txt = "PAUSED";
        DrawText(pause_txt, GetScreenWidth() - MeasureText(pause_txt, 20), 0, 20, RED);
    }

    float framePerSecond = getFPS();
    float tickPerSecond = getTPS();

    GuiSlider((Rectangle){ GetScreenWidth() / 2.f - 100.f, GetScreenHeight() - 20.f, 200, 16 }, "FPS 1", "100%", &framePerSecond, 1, 300);
    GuiSlider((Rectangle){ GetScreenWidth() / 2.f - 100.f, GetScreenHeight() - 50.f, 200, 16 }, "TPS 0", "100%", &tickPerSecond, 0, 1000);
    
    if(static_cast<int>(framePerSecond) != getFPS())
        setFPS(framePerSecond);

    if(static_cast<int>(tickPerSecond) != getTPS())
        setTPS(tickPerSecond);
}