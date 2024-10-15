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

            void setTPS(float tps);
            void setFPS(float fps);

            const inline int getTPS(){return 1.0/m_tickPeriod;};
            const inline int getFPS(){return 1.0/m_framePeriod;};

            void setPause(bool pause);
            bool isPaused() const { return m_pause; };

            virtual void drawFrame();
            virtual void drawUI();
            virtual void updateTick();

            /**
             * Executé après le démarrage de raylib
             */
            virtual void init() {};

            /**
             * Executé avant l'arrêt de raylib
             */
            virtual void unload() {};

        protected:
            Camera2D m_camera;
        private:

            void drawAll();
            void updateUI();

            double m_tickPeriod;
            double m_framePeriod;

            int m_lastFrameCounter;
            int m_lastTickCounter;

            bool m_noDelay;

            bool m_pause;

            RenderTexture2D m_renderer;
            RenderTexture2D m_gui_renderer;
    };
}

#endif