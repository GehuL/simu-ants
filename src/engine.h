#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "config.h"

#include "raylib.h"
#include <string>

#include "profiling.h"

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

            Camera2D getCamera() const { return m_camera; };

            void setPause(bool pause);
            bool isPaused() const { return m_pause; };

            virtual void drawFrame();
            virtual void drawUI();
            virtual void updateTick();

            int getWidth() const { return m_renderer.texture.width; };
            int getHeight() const { return m_renderer.texture.height; };
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
            Profiler m_profiler;
        private:

            void drawAll();
            void updateUI();

            double m_tickPeriod;
            double m_framePeriod;

            double m_lastUpdateProfiler;

            bool m_noDelay;

            bool m_pause;

            RenderTexture2D m_renderer;
            RenderTexture2D m_gui_renderer;

            ProfileData m_tickProfile;
            ProfileData m_frameProfile;
    };
}

#endif