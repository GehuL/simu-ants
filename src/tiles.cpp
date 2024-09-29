#include "tiles.h"
#include <stdexcept>

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

}

bool const Grid::isValid(int x, int y)
{
    return x >= 0 && x < m_gridWidth && y >= 0 && y < m_gridWidth;
}

void const Grid::check(int x, int y)
{
    if(!isValid(x, y))
        throw std::range_error("Out of range");
}

void Grid::reset()
{
    if(m_grid != nullptr)
        delete[] m_grid;
     
    m_grid = new Tile[m_gridWidth*m_gridWidth]();
}

void Grid::setTile(Tile tile, int x, int y)
{
    check(x, y);
    m_grid[y*m_gridWidth + x] = tile;
}

Tile const Grid::getTile(int x, int y)
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

Vector2i const Grid::toTileCoord(float x, float y)
{
    int tileX = x / getTileSize();
    int tileY = y / getTileSize();
    return (Vector2i) {tileX, tileY};
}
