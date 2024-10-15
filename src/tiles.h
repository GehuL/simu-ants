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

            void unload();

            void update();
            void draw();

            friend void to_json(json& json, const Grid& grid);
            friend void from_json(const json& json, Grid& grid);

            void reset();



            /** @brief Renvoie la tuile en fonction de l'index x et y de la grille
              * @param check Active la vérification la validité des index. Le désactiver est à vos risque et péril
              * @return Renvoie la tuile à la position en paramètre. La tuile est de type BORDER si la positions 
              * est en dehors de la grille et que le check est activé.
              */
            template<bool check = true>
            Tile getTile(Vector2i pos) const
            {
                if constexpr(check)
                {
                    if(!isValid(pos.x, pos.y))
                        return BORDER;
                }
                
                return m_grid[pos.y*m_gridWidth + pos.x];
            }
            
            /** @brief Renvoie une tuile en fonction des coordonnées globales x et y
             *  @param _check Active la vérification la validité des index. Le désactiver est à vos risque et péril
             * @return Renvoie la tuile à la position en paramètre. La tuile est de type BORDER si la positions 
             * est en dehors de la grille et que le _check est activé.
             */
            template<bool _check = true>
            Tile getTile(Vector2f pos) const
            {
                const int tileX = pos.x / getTileSize();
                const int tileY = pos.y / getTileSize();

                return getTile<_check>((Vector2i) {tileX, tileY} );
            }  

            /** @brief Pose une tuile a partir des coordonnées globales x et y
             *  @param _check Active la vérification la validité des index. Le désactiver est à vos risque et péril
             * Si la vérification est activé et que les coordonnées sont invalides, la grille n'est pas modifié.
             */
            template<bool _check = true>
            void setTile(Tile tile, float x, float y)
            {
                const int tileX = x / getTileSize();
                const int tileY = y / getTileSize();

                if constexpr(_check)
                {
                    if(getTile<_check>((Vector2i) {tileX, tileY}).type != Type::BORDER)
                        setTile<_check>(tile, tileX, tileY);
                }else
                {
                    setTile<_check>(tile, tileX, tileY);
                }
    
            }

            /** @brief Pose une tuile a partir des coordonnées de la grille
             *  @param _check Active la vérification la validité des index. Le désactiver est à vos risque et péril
             */
            template<bool _check = true>
            void setTile(Tile tile, int x, int y)
            {
                if constexpr(_check)
                {
                    check(x, y);
                }

                m_grid[y*m_gridWidth + x] = tile;
                ImageDrawPixel(&m_img, x, y, tile.color);
            }

            Vector2i toTileCoord(float x, float y) const;

            int getGridWidth() const { return m_gridWidth; };
            int getTileSize() const { return m_tileSize; };

            int getTileNumber() const { return m_gridWidth*m_gridWidth; };

            bool isValid(int x, int y) const;
            void check(int x, int y) const;
        
        private:
            int m_gridWidth;
            int m_tileSize;

            Tile *m_grid;

            Texture2D m_tex;
            Image m_img;
    };

    void to_json(json& json, const Grid& grid);
    void from_json(const json& json, Grid& grid);
}

#endif