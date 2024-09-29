#ifndef __TILES_H__
#define __TILES_H__

#include "raylib.h"

namespace simu
{
    enum class Type
    {
        AIR,
        WALL,
        FOOD,
        PHEROMONE,
        BORDER,  
    };

    struct Tile
    {
        Tile(Type type=Type::AIR, Color color=WHITE) : type(type), color(color) {};
        Tile& operator =(Tile& t) { this->type = t.type; this->color = t.color; return *this; };

        Type type;
        Color color;

    };

    static const Tile AIR = Tile(Type::AIR, WHITE);
    static const Tile WALL = Tile(Type::WALL, BROWN);
    static const Tile FOOD = Tile(Type::FOOD, YELLOW);
    static const Tile PHEROMONE = Tile(Type::AIR, PINK);

    class Grid
    {
        public:
            Grid(const int gridWidth, const int gridSize);
            ~Grid();

            void update();
            void draw();

            void reset();
            void setTile(Tile tile, int x, int y);

            // Renvoie la tuile en fonction de l'index x et y
            Tile const getTile(int x, int y);

            // Renvoie une tuile en fonction des coordonnées x et y
            Tile getTile(float x, float y);
            void setTile(Tile tile, float x, float y);
            Vector2 const toTileCoord(float x, float y);

            int const getGridWidth() { return m_gridWidth; };
            int const getTileSize() { return m_tileSize; };

            int constexpr getTileNumber() { return m_gridWidth*m_gridWidth; };

            bool const isValid(int x, int y);
            void const check(int x, int y);
        
        private:
            const int m_gridWidth;
            const int m_tileSize;
            Tile *m_grid;
    };
}

#endif