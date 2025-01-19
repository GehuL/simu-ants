#include "engine.h"
#include "utils.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

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
    double lastProfilerTime = 0;
    double lastGUITime = 0;

    double lag = 0.0;

    int tickCounter = 0;
    int frameCounter = 0;

    InitWindow(screenWidth, screenHeight, title.c_str());
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    
    m_renderer = LoadRenderTexture(screenWidth, screenHeight);
    m_gui_renderer = LoadRenderTexture(screenWidth, screenHeight);

    init();

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
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
                m_profiler.begin("tick");
                updateTick();
                m_profiler.end();
                TraceLog(LOG_DEBUG, "%lf", m_profiler["tick"]->elapsed);

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
        bool b_updateUI = uiDelta >=  1.0 / 30.0; // Keep ui at a consistant frame rate

        // Draw frame OR update UI
        if(b_updateUI || b_drawAll)
        {
            m_profiler.begin("frame");

            BeginDrawing();

            if(b_drawAll)
            {
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
                lastGUITime = GetTime();
                updateUI();
                PollInputEvents();
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

        // Mise à jour profiler
        if(GetTime() - m_lastUpdateProfiler > 1.0)
        {
            m_lastUpdateProfiler = GetTime();
        
            m_tickProfile = *m_profiler["tick"];
            m_frameProfile = *m_profiler["frame"];
            m_profiler.resetAll();
        }
      
        // Différence de temps libre entre update et draw (drawDelta inclue le temps de l'affichage de l'UI)
        double waitTime = 0.0;
        if(m_pause)
            waitTime = m_framePeriod - delta;
        else
            waitTime = MIN(m_framePeriod - delta, m_tickPeriod - delta);
       
        if(waitTime >= 0.0) // Il reste du temps pour mettre en pause 
        {
            // Désactiver le waitTime permet d'augmenter la priorité du processus 
            WaitTime(waitTime);
            // TRACELOG(LOG_INFO, "waitTime: %lf ms", waitTime * 1000.0);
        }
    }
    unload();

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

    BeginTextureMode(m_gui_renderer);

    ClearBackground((Color){255, 255, 255, 0});

    drawUI();

    DrawText(TextFormat("%d FPS (%.2lfms)\n%d TPS\nTick load: %.2lf/%d ms (%.1f%%)", 
    m_frameProfile.sampleCnt, std::chrono::duration_cast<std::chrono::milliseconds>(m_frameProfile.calculAverage()),
    m_tickProfile.sampleCnt, std::chrono::duration_cast<std::chrono::milliseconds>(m_tickProfile.calculAverage()), 
    static_cast<int>(m_tickPeriod * 1000) ,
    100 * m_tickProfile.average / m_tickPeriod), 5, 0, 20, GREEN);

    if(m_pause)
    {
        const char* pause_txt = "PAUSED";
        DrawText(pause_txt, GetScreenWidth() - MeasureText(pause_txt, 20), 0, 20, RED);
    }

    float framePerSecond = getFPS();
    float tickPerSecond = getTPS();

    GuiSlider((Rectangle){ GetScreenWidth() / 2.f - 100.f, GetScreenHeight() - 20.f, 200, 16 }, TextFormat("FPS %d", static_cast<int>(framePerSecond)), "100%", &framePerSecond, 1, 300);
    GuiSlider((Rectangle){ GetScreenWidth() / 2.f - 100.f, GetScreenHeight() - 50.f, 200, 16 }, TextFormat("TPS %d", static_cast<int>(tickPerSecond)), "100%", &tickPerSecond, 0, 1000);
    
    if(static_cast<int>(framePerSecond) != getFPS()) {
        setFPS(framePerSecond);
    }

    if(static_cast<int>(tickPerSecond) != getTPS() && !m_noDelay) {
        setTPS(tickPerSecond);
    }
    
    bool lastStateNoDelay = m_noDelay;
    GuiCheckBox((Rectangle){ GetScreenWidth() / 2.f + 150, GetScreenHeight() - 50.f, 16, 16 }, "No limit", &m_noDelay);
    if(m_noDelay && !lastStateNoDelay) {
        setTPS(999999999);
    }

    EndTextureMode();
}
