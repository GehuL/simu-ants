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

    int tickCounter = 0;
    int frameCounter = 0;

    InitWindow(screenWidth, screenHeight, title.c_str());
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    
    m_renderer = LoadRenderTexture(screenWidth, screenHeight);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update logic
        double updateDelta = GetTime() - lastUpdateTime;
        if(updateDelta >= m_tickPeriod)
        {
            double now = GetTime();
            lastUpdateTime = now;
            updateTick();
            tickCounter++;
        }

        // Draw frame
        double drawDelta = GetTime() - lastDrawTime;

        if(drawDelta >= m_framePeriod)
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
                frameCounter++;
            }
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

void Engine::set_fps(int fps)
{
    this->m_framePeriod = 1.0 / static_cast<double>(fps);
}

void Engine::set_tps(int tps)
{
    this->m_tickPeriod = 1.0 / static_cast<double>(tps);
}

void Engine::drawAll()
{
    BeginDrawing();
        BeginTextureMode(m_renderer);

            ClearBackground(RAYWHITE);

            BeginMode2D(m_camera);
                drawFrame();
            EndMode2D();


        EndTextureMode();
            
        DrawTexturePro(m_renderer.texture, {0, 0, (float)m_renderer.texture.width, -(float)m_renderer.texture.height}, 
                                           {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                                            {0, 0}, 0, WHITE);
        drawUI();

    EndDrawing();

#ifdef SUPPORT_CUSTOM_FRAME_CONTROL
    SwapScreenBuffer();
    PollInputEvents();
#endif
}

void Engine::updateTick()
{

}

void Engine::drawFrame()
{
}

void Engine::drawUI()
{
    DrawText(TextFormat("%d FPS\n%d TPS", m_lastFrameCounter, m_lastTickCounter), 5, 0, 20, GREEN);

    float framePerSecond = get_fps();
    float tickPerSecond = get_tps();

    GuiSlider((Rectangle){ GetScreenWidth() / 2.f - 100.f, GetScreenHeight() - 20.f, 200, 16 }, "FPS 1", "100%", &framePerSecond, 1, 300);
    GuiSlider((Rectangle){ GetScreenWidth() / 2.f - 100.f, GetScreenHeight() - 50.f, 200, 16 }, "TPS 0", "100%", &tickPerSecond, 0, 1000);
    
    if(static_cast<int>(framePerSecond) != get_fps())
        set_fps(framePerSecond);

    if(static_cast<int>(tickPerSecond) != get_tps())
    {
        m_noDelay = false;
        if(tickPerSecond >= 1000)
        {
            m_noDelay = true;
            tickPerSecond = 999999999;
        }
        set_tps(tickPerSecond);
    }
}