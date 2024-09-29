#ifndef __ANT_H__
#define __ANT_H__

#include <map>
#include <string>

#include "entity.h"

namespace simu
{
    class Ant : public Entity
    {
        public:
            Ant();
            virtual ~Ant(){};

            void update() override;
            void draw() override;

            void eat();
    
            float const getLife() { return m_life; };

        private:
            float m_posX, m_posY;
            float m_life;
    };
}

#endif