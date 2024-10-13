#include "tiles.h"

#include <memory.h>
#include "external/base64.h"

using namespace simu;

Grid::Grid(const int gridWidth, const int tileSize) : m_gridWidth(gridWidth), m_tileSize(tileSize)
{
    reset();
}

Grid::~Grid()
{
    if(m_grid != nullptr)
        delete[] m_grid;
}

void Grid::draw()
{
    for(int y = 0; y < m_gridWidth; y++)
    {
        for(int x = 0; x < m_gridWidth; x++)
        {
            DrawRectangle(x*m_tileSize, y*m_tileSize, m_tileSize, m_tileSize, m_grid[y*m_gridWidth + x].color);
            DrawRectangleLinesEx((Rectangle){static_cast<float>(x*m_tileSize), static_cast<float>(y*m_tileSize), static_cast<float>(m_tileSize) + 1, static_cast<float>(m_tileSize) + 1}, 0.5f, GRAY);
        }
    }
}

void Grid::update()
{
    for(int i = 0; i < getTileNumber(); i++)
    {
        Tile* tile = &m_grid[i];
        switch(tile->type)
        {
            case Type::PHEROMONE:
                tile->color.a -= 1;
                if(tile->color.a <= 0)
                    *tile = AIR; 
            break;
            default:
                continue;
        }
    }

}

bool Grid::isValid(int x, int y) const
{
    return x >= 0 && x < m_gridWidth && y >= 0 && y < m_gridWidth;
}

void Grid::check(int x, int y) const 
{
    if(!isValid(x, y))
        throw std::range_error("Out of range");
}

void Grid::reset()
{
    if(m_grid != nullptr)
        delete[] m_grid;
     
    m_grid = new Tile[m_gridWidth*m_gridWidth] {AIR};
}

void Grid::setTile(Tile tile, int x, int y)
{
    check(x, y);
    m_grid[y*m_gridWidth + x] = tile;
}

Tile Grid::getTile(int x, int y) const
{
    if(!isValid(x, y))
        return BORDER;
    else
       return m_grid[y*m_gridWidth + x];
}

Tile Grid::getTile(float x, float y)
{
    int tileX = x / getTileSize();
    int tileY = y / getTileSize();

    return getTile(tileX, tileY);
}

void Grid::setTile(Tile tile, float x, float y)
{
    int tileX = x / getTileSize();
    int tileY = y / getTileSize();

    if(getTile(tileX, tileY).type != Type::BORDER)
        setTile(tile, tileX, tileY);
}

Vector2i Grid::toTileCoord(float x, float y) const
{
    int tileX = x / getTileSize();
    int tileY = y / getTileSize();
    return (Vector2i) {tileX, tileY};
}

void simu::to_json(json &json, const Grid &grid)
{
    unsigned char* encoded = base64_encode((const unsigned char*)(grid.m_grid), grid.getTileNumber() * sizeof(Tile), NULL);
    std::string data((char*) encoded);
    json["width"] = grid.getGridWidth();
    json["data"] = data;
    delete[] encoded;
}

void simu::from_json(const json & json, Grid & grid)
{
    auto rowdata = json.at("data");
    
    if(!rowdata.is_string())
        throw std::runtime_error("Impossible de charger la grille: La grille est corrompu");

    int gridWidth = json.at("width");
    
    std::string data;
    rowdata.get_to(data);

    size_t decoded_len = 0;
    unsigned char* decoded = base64_decode((const unsigned char*) data.c_str(), data.length(), &decoded_len);

    int tilesNumber = decoded_len / sizeof(Tile);
    if(gridWidth*gridWidth != tilesNumber) // Test que la grille est bien carré
        throw std::runtime_error("Impossible de charger la grille: La grille est corrompu");
    
    if(grid.m_grid)
        delete[] grid.m_grid;
    
    grid.m_grid = reinterpret_cast<Tile*>(decoded);
}
