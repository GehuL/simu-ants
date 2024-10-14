#include "tiles.h"
#include "utils.h"
using namespace simu;

// On utilise le système d'allocation de raylib pour cette class! 


Grid::Grid(const int gridWidth, const int tileSize) : m_gridWidth(gridWidth), m_tileSize(tileSize)
{
    reset();
}

Grid::~Grid()
{
    if(m_grid != nullptr)
        MemFree(m_grid);
}

void Grid::draw()
{
    // DrawTexture();

    for(int y = 0; y < m_gridWidth; y++)
    {
        for(int x = 0; x < m_gridWidth; x++)
        {
            DrawRectangle(x*m_tileSize, y*m_tileSize, m_tileSize, m_tileSize, m_grid[y*m_gridWidth + x].color);
        }
    }
    // TODO: Utiliser une Texture2D pour optimiser le rendu

    // Draw vertical lines
    for(int i = 0; i < m_gridWidth; i++)
    {
        DrawLine(i*m_tileSize, 0, i*m_tileSize, m_tileSize*m_gridWidth, GRAY);
        DrawLine(0, i*m_tileSize, m_tileSize*m_gridWidth, i*m_tileSize, GRAY);
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
        MemFree(m_grid);
     
    m_grid = (Tile*) MemAlloc(m_gridWidth*m_gridWidth*sizeof(Tile));
}

void Grid::setTile(Tile tile, int x, int y)
{
    check(x, y);
    m_grid[y*m_gridWidth + x] = tile;
}

Tile Grid::getTile(Vector2i pos) const
{
    if(!isValid(pos.x, pos.y))
        return BORDER;
    return m_grid[pos.y*m_gridWidth + pos.x];
}

Tile Grid::getTile(Vector2f pos) const
{
    int tileX = pos.x / getTileSize();
    int tileY = pos.y / getTileSize();

    return getTile((Vector2i) {tileX, tileY} );
}

void Grid::setTile(Tile tile, float x, float y)
{
    int tileX = x / getTileSize();
    int tileY = y / getTileSize();

    if(getTile((Vector2i) {tileX, tileY}).type != Type::BORDER)
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
    // TODO: Compresser les données
    int compressed_len = 0, encoded_len = 0;
    unsigned char* compressed = CompressData((const unsigned char*)(grid.m_grid), grid.getTileNumber() * sizeof(Tile), &compressed_len);
    char* encoded = EncodeDataBase64((const unsigned char*)(compressed), compressed_len, &encoded_len);
    encoded[encoded_len - 1] = '\0';
    
    json["width"] = grid.getGridWidth();
    json["data"] = encoded;

    // TRACELOG(LOG_INFO, "%s", json["data"].dump());
    // 16540, 12420
    MemFree(encoded);
    MemFree(compressed);
}

void simu::from_json(const json & json, Grid & grid)
{
    // TODO: Décompresser les données
    auto rowdata = json.at("data");
    
    if(!rowdata.is_string())
        throw std::runtime_error("Impossible de charger la grille: La grille est corrompu");

    int gridWidth = json.at("width");
    
    std::string data;
    rowdata.get_to(data);

    int decoded_len = 0, decompressed_len = 0;
    unsigned char* decoded = DecodeDataBase64((const unsigned char*) data.c_str(), &decoded_len);
    unsigned char* decompressed = DecompressData(decoded, decoded_len, &decompressed_len);

    if(decoded != NULL)
        MemFree(decoded);

    int tilesNumber = decompressed_len / sizeof(Tile);
    if(gridWidth*gridWidth != tilesNumber) // Test que la grille est bien carré
        throw std::runtime_error("Impossible de charger la grille: La grille est corrompu");
    
    if(grid.m_grid)
        MemFree(grid.m_grid);
    
    grid.m_gridWidth = gridWidth;
    grid.m_grid = reinterpret_cast<Tile*>(decompressed);
}
