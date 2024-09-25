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
            int run(int screenWidth, int screenHeight, std::string title);

            void set_tps(int tps);
            void set_fps(int fps);

            const inline int get_tps(){return 1.0/m_tickPeriod;};
            const inline int get_fps(){return 1.0/m_framePeriod;};

            static Engine engine;

        private:
            Engine();

            void updateTick();
            void updateFrame();

            double m_tickPeriod;
            double m_framePeriod;

            int m_lastFrameCounter;
            int m_lastTickCounter;

            bool m_noDelay;

            Camera2D m_camera;
    };
}


#endif