#ifndef __OTHERS_H__
#define __OTHERS_H__

#include "raylib.h"

#include <vector>
#include <iterator>

namespace simu
{
    typedef struct Vector2i
    {
        int x;
        int y;
    }Vector2i;

    typedef Vector2 Vector2f;

    // Fonction optimisé pour retiré un élément d'une liste (l'ordre des éléments n'est pas respecté)
    // source: https://dev.to/dinhluanbmt/c-deleteerase-item-from-vector-in-o1-time-3okb
    inline void erase(std::vector<int> &v, typename std::vector<int>::iterator it) 
    {
        if ( it != v.end()) {
            if ( it < v.end() - 1)
                *it = std::move( v.back() );
            v.pop_back();
        }
    }

}

#endif 