#ifndef __TILES_H__
#define __TILES_H__

#include "raylib.h"
#include "others.h"
#include "external/json.hpp"

namespace simu
{
    using json = nlohmann::json;

    enum class Type
    {
        AIR,
        GROUND,
        FOOD,
        PHEROMONE,
        BORDER,  
    };

    struct Tile
    {
        Type type;
        Color color;
    };

    const Tile AIR = (Tile) {Type::AIR, WHITE};
    const Tile GROUND = (Tile) {Type::GROUND, BROWN};
    const Tile FOOD = (Tile) {Type::FOOD, YELLOW};
    const Tile PHEROMONE = (Tile) {Type::PHEROMONE, PINK};
    const Tile BORDER = (Tile) {Type::BORDER, WHITE};

    class Grid
    {
        public:
            Grid(const int gridWidth, const int tileSize);
            ~Grid();

            void update();
            void draw();

            friend void to_json(json& json, const Grid& grid);
            friend void from_json(const json& json, Grid& grid);

            void reset();
            void setTile(Tile tile, int x, int y);

            // Renvoie la tuile en fonction de l'index x et y
            Tile getTile(int x, int y) const;

            // Renvoie une tuile en fonction des coordonnées x et y
            Tile getTile(float x, float y);
            void setTile(Tile tile, float x, float y);
            Vector2i toTileCoord(float x, float y) const;

            int getGridWidth() const { return m_gridWidth; };
            int getTileSize() const { return m_tileSize; };

            int getTileNumber() const { return m_gridWidth*m_gridWidth; };

            bool isValid(int x, int y) const;
            void check(int x, int y) const;
        
        private:
            const int m_gridWidth;
            const int m_tileSize;
            Tile *m_grid;
    };

    void to_json(json& json, const Grid& grid);
    void from_json(const json& json, Grid& grid);
}

#endif