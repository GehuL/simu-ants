#ifndef __TILES_H__
#define __TILES_H__

#include "raylib.h"
#include "engine.h"
#include "types.h"
#include "../external/json.hpp"

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

    // TODO: Optimiser l'occupation en mémoire de la grille 
    struct Tile
    {
        Type type;
        Color color;
    };

    const Tile AIR = (Tile) {Type::AIR, WHITE};
    const Tile GROUND = (Tile) {Type::GROUND, BROWN};
    const Tile WALL = (Tile) {Type::GROUND, BLACK}; // Considère un mur comme le sol (utilisé pour le labyrinthe)
    const Tile FOOD = (Tile) {Type::FOOD, GREEN};
    const Tile PHEROMONE = (Tile) {Type::PHEROMONE, PINK};
    const Tile BORDER = (Tile) {Type::BORDER, WHITE};

    Tile fromColor(const Color& color);
    bool operator==(const Color &c1, const Color &c2);

    constexpr Color GRID_COLOR { 130, 130, 130, 115 }; 

    class Grid
    {
        public:
            Grid(const int tileSize);
            ~Grid();

            void unload();

            void update();
            void draw();

            friend void to_json(json& json, const Grid& grid);
            friend void from_json(const json& json, Grid& grid);

            /** @brief Charge la grille depuis une image au format supporté par la version de raylib (SVG, PNG, JPG, BMP, GIF...). L'image doit être carré
             *  @param file Chemin et nom du fichier avec extension
             *  @throw std::runtime_eror si le fichier n'est pas trouvé, ne peut pas à être lu, l'extension n'est pas supporté ou que l'image n'est pas carré.
             */
            void fromImage(const std::string& file);

            /** @brief Construit une grille constitué d'air par défaut 
             *  @param gridWidth Largeur et longueur de la grille
             */
            void init(int gridWidth);

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
            Tile getTile(Vec2f pos) const
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

                setTile<_check>(tile, tileX, tileY);
            }

            /** @brief Pose une tuile a partir des coordonnées de la grille
             *  @param _check Active la vérification la validité des index. Le désactiver est à vos risque et péril
             */
            template<bool _check = true>
            void setTile(Tile tile, int x, int y)
            {
                Tile tileOn = getTile<_check>((Vector2i) {x, y});
               
                if constexpr(_check) // Si check est activé, sinon il y aurait du avoir une erreur
                    if(tileOn.type == Type::BORDER)
                        return;    

                const int idx = y*m_gridWidth + x; 
                
                // On évite d'ajouter un index qui est déjà présent
                if(tile.type == Type::PHEROMONE && tileOn.type != Type::PHEROMONE) 
                {
                     m_updateBuff.push_back(idx);
                }
               
                setTile(tile, idx);
            }

            /** @brief Trouve un chemin depuis start a dest. L'algorithme A* est utilisé.
             */
            std::vector<Vec2i> findPath(Vec2i start, Vec2i dest);

            /* @brief Renvoie le nombre de case du chemin entre start et dest utilisant A*.
             */
            int pathDistance(Vec2i start, Vec2i dest);

            Vector2i toTileCoord(float x, float y) const;
            Vector2i toTileCoord(Vec2f pos) const;


            int getGridWidth() const { return m_gridWidth; };
            int getTileSize() const { return m_tileSize; };

            int getTileNumber() const { return m_gridWidth*m_gridWidth; };

            bool isValid(int x, int y) const;
            void check(int x, int y) const;
        
            friend void compressGrid(const Grid& grid, std::string& output);
            friend void decompressGrid(Grid& grid, std::string& data, int gridWidth);
        
        private:

            // Met à jour le buffer du rendu et la grille. Ne vérifie pas l'index.
            void setTile(Tile, int index); 

            int m_gridWidth;
            int m_tileSize;

            Tile *m_grid;

            // Buffer pour savoir quelles sont les cases à mettre à jour au lieu de faire du polling sur toutes les cases
            std::vector<int> m_updateBuff;

            Texture2D m_tex;    // Buffer de rendu pour optimiser les FPS
            Image m_img;        // Buffer de rendu pour optimiser les FPS 
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