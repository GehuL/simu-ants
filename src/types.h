#ifndef __OTHERS_H__
#define __OTHERS_H__

#include "raylib.h"

#include <vector>
#include <iterator>
#include <functional>

namespace simu
{
    template<typename T>
    struct Vec2
    {
        T x;
        T y;

        Vec2() : x(0), y(0) {};
        Vec2(T _x, T _y) : x(_x), y(_y) {}; 
        Vec2(const Vec2& vec) : x(vec.x), y(vec.y) {};
        
        T mag() const { return x*x + y*y; };

        // Distance de Manhattan
        T distance(Vec2& vec) const { return std::abs(vec.x - x) + std::abs(vec.y - y); };
        bool operator==(const Vec2& vec) const { return x == vec.x && y == vec.y; };
        bool operator!=(const Vec2& vec) const { return !operator==(vec); };
        Vec2 operator+(const Vec2& vec) const { return Vec2{x + vec.x, y + vec.y}; };
        size_t operator()() const { return x*x + y*y; };
        size_t operator()(const Vec2& vec) const { return vec(); }; // Utile pour les fonctions de Hash
        bool operator<(const Vec2& vec) const {return mag() < vec.mag(); };
    };

    template<typename T>
    struct VecHasher
    {
        size_t operator()(const Vec2<T> &vec) const 
        {
            return std::hash<T>()(vec.x) ^ (std::hash<T>()(vec.y) << 1);
        }
    };

    typedef Vec2<int> Vector2i, Vec2i;
    typedef Vec2<float> Vec2f;
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

    // Direction en radian par rapport au cercle trigonometrique
    class Direction
    {
            float mAngle;
        public:
            Direction(float angle) : mAngle(angle) {};
            operator float() const { return mAngle; };
    };

    const Direction EAST(0.f);
    const Direction NORTH(PI / 2.f);
    const Direction WEST(PI);
    const Direction SOUTH(3.f * PI / 2.f);
}

#endif 