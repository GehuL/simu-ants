#ifndef __TYPES_H__
#define __TYPES_H__

#include "raylib.h"

#include <vector>
#include <iterator>
#include <functional>
#include <ostream>
#include <math.h>

namespace simu
{
    template<typename T>
    struct Vec2
    {
        T x;
        T y;

        Vec2() : x(0), y(0) {};
        Vec2(T _x, T _y) : x(_x), y(_y) {};

        template<typename V>
        Vec2(const Vec2<V>& vec) : x(vec.x), y(vec.y) {};
        Vec2(const Vector2& vec): x(vec.x), y(vec.y) {}; // Raylib compatiblity
        

        float angle(const Vec2<T>& vec) const { return std::acos((x*vec.x + y*vec.y) / (std::sqrt(mag()) * std::sqrt(vec.mag()))); };

        T mag() const { return x*x + y*y; };

        T euclide(const Vec2<T>& vec) const { return std::sqrt((vec.x - x)*(vec.x - x) + (vec.y - y)*(vec.y - y)); };
        T manhattan(const Vec2<T>& vec) const { return std::abs(vec.x - x) + std::abs(vec.y - y); };  // Distance de Manhattan

        bool operator==(const Vec2<T>& vec) const { return x == vec.x && y == vec.y; };
        bool operator!=(const Vec2<T>& vec) const { return !operator==(vec); };

        Vec2<T> operator+(const Vec2<T>& vec) const { return Vec2<T>{x + vec.x, y + vec.y}; };
        Vec2<T> operator-(const Vec2<T>& vec) const { return Vec2<T>{x - vec.x, y - vec.y}; };
        
        Vec2<T> operator*(const float scalar) const { return Vec2<T>{static_cast<T>(x * scalar), static_cast<T>(y * scalar)}; };
        Vec2<T> operator/(const float scalar) const { return Vec2<T>{static_cast<T>(x / scalar), static_cast<T>(y / scalar)}; };

        Vec2<T> operator+=(const Vec2<T>& vec) { x += vec.x; y += vec.y; return *this; };
    
        size_t operator()() const { return x*x + y*y; };

        bool operator<(const Vec2<T>& vec) const {return mag() < vec.mag(); };

        template<typename V>
        friend std::ostream& operator<<(std::ostream& os, const Vec2<V>& vec);
    };

    template<typename V>
    std::ostream& operator<<(std::ostream& os, const Vec2<V>& vec)
    {
        os << '{' << vec.x << ',' << vec.y << '}'; 
        return os;
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
    typedef Vec2<float> Vector2f, Vec2f;

    const Vec2i LEFT(-1, 0);
    const Vec2i RIGHT(1, 0);
    const Vec2i UP(0, -1);
    const Vec2i DOWN(0, 1);
    
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