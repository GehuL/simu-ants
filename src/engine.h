#ifndef __ENGINE_H__
#define __ENGINE_H__

#define RAYGUI_IMPLEMENTATION

#include "config.h"
#include "raylib.h"
#include <string>

namespace simu
{
    class Engine
    {
        public:
            Engine();
            
            int run(int screenWidth, int screenHeight, std::string title);

            void setTPS(int tps);
            void setFPS(int fps);

            const inline int getTPS(){return 1.0/m_tickPeriod;};
            const inline int getFPS(){return 1.0/m_framePeriod;};

            void setPause(bool pause);

            virtual void drawFrame();
            virtual void drawUI();
            virtual void updateTick();

        protected:
            Camera2D m_camera;
        private:

            void drawAll();

            double m_tickPeriod;
            double m_framePeriod;

            int m_lastFrameCounter;
            int m_lastTickCounter;

            bool m_noDelay;

            bool m_pause;

            RenderTexture2D m_renderer;
    };
}

#endif