#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "config.h"
#include "raylib.h"
#include "string"

namespace simu
{
    class Engine
    {
        public:
            Engine();
            
            int run(int screenWidth, int screenHeight, std::string title);

            void set_tps(int tps);
            void set_fps(int fps);

            const inline int get_tps(){return 1.0/m_tickPeriod;};
            const inline int get_fps(){return 1.0/m_framePeriod;};

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

            RenderTexture2D m_renderer;
    };
}

#endif