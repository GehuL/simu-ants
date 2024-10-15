#ifndef __TILES_H__
#define __TILES_H__

#include "raylib.h"
#include "engine.h"
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

    // TODO: Implémenter des chunks pour optimiser le rendu graphique
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

            // Renvoie la tuile en fonction de l'index x et y de la grille
            Tile getTile(Vector2i) const;
            // Renvoie une tuile en fonction des coordonnées globales x et y
            Tile getTile(Vector2f) const;
            
            void setTile(Tile tile, float x, float y);
            Vector2i toTileCoord(float x, float y) const;

            int getGridWidth() const { return m_gridWidth; };
            int getTileSize() const { return m_tileSize; };

            int getTileNumber() const { return m_gridWidth*m_gridWidth; };

            bool isValid(int x, int y) const;
            void check(int x, int y) const;
        
            friend void compressGrid(const Grid& grid, std::string& output);
            friend void decompressGrid(Grid& grid, std::string& data, int gridWidth);
        
        private:

            int m_gridWidth;
            int m_tileSize;

            Tile *m_grid;
    };

    void to_json(json& json, const Grid& grid);
    void from_json(const json& json, Grid& grid);

    /**@brief Encode (base 64) et compresse (DEFLATE algo) les données de la grille pour être mis dans un txt
         * @param output Les données compressés et encodés
         * @grid La grille à compresser
         * @throw Lève une exception si une erreur à eu lieu (mémoire insuffisante, erreur de compression)
         * Si une erreur à lieu, la grille n'est pas modifié
         */
    void compressGrid(const Grid& grid, std::string& output);

    /**@brief Decode (base 64) et decompresse (DEFLATE algo) des donnéespour charger la grille
     * @param data Les données compressées et encodées d'une grille
     * @param La largeur de la grille en nombre de tuile
     * @throw Lève une exception si une erreur a eu lieu (mémoire insuffisante, erreur de decompression, données corrompues)
     * Si une erreur à lieu, la grille n'est pas modifié
     */
    void decompressGrid(Grid& grid, std::string& data, int gridWidth);
}

#endif