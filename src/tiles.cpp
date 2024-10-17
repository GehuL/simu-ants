#include "tiles.h"
#include "utils.h"

using namespace simu;

// On utilise le système d'allocation de raylib pour cette class! 


Grid::Grid(const int gridWidth, const int tileSize) : m_gridWidth(gridWidth), m_tileSize(tileSize)
{

}

Grid::~Grid()
{
    if(m_grid != nullptr)
        MemFree(m_grid);
}

void Grid::unload()
{
    if(m_grid != nullptr)
        MemFree(m_grid);
    
    m_updateBuff.clear();
    m_grid = nullptr;

    UnloadImage(m_img);
    UnloadTexture(m_tex);
}

void Grid::draw()
{
    // TODO: Utiliser UpdateTextureRec pour mettre à jour que certains pixels
    UpdateTexture(m_tex, m_img.data);

    const int gridWidthPixel = getTileSize() * m_gridWidth;
    DrawTexturePro(m_tex, (Rectangle) {0, 0, (float) m_tex.width, (float) m_tex.width}, 
                        (Rectangle) {0, 0, (float) gridWidthPixel, (float) gridWidthPixel}, 
                        (Vector2){0, 0}, 0, WHITE);

    //Draw vertical lines
    for(int i = 0; i < m_gridWidth; i++)
    {
        DrawLine(i*m_tileSize, 0, i*m_tileSize, m_tileSize*m_gridWidth, Color{ 130, 130, 130, 115 });
        DrawLine(0, i*m_tileSize, m_tileSize*m_gridWidth, i*m_tileSize, Color{ 130, 130, 130, 115 });
    }
}

void Grid::update()
{
    for(auto it = m_updateBuff.begin(); it != m_updateBuff.end(); it++)
    {
        int index = *it;
        Tile* tile = &m_grid[index];

        switch(tile->type)
        {
            case Type::PHEROMONE:
               
                if(tile->color.a <= 0)
                {
                    *tile = AIR;
                    setTile(AIR, index);
                    erase(m_updateBuff, it);
                    it--;
                }else
                {
                    tile->color.a -= 1;
                    setTile(*tile, index);
                }
            break;
            default:
                erase(m_updateBuff, it);
                it--;
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

void Grid::setTile(Tile tile, int index)
{
    m_grid[index] = tile;
    ImageDrawPixel(&m_img, index % m_gridWidth, index / m_gridWidth, tile.color); // Met à jour le buffer de rendu
}

void Grid::reset()
{
    unload();

    m_grid = (Tile*) MemAlloc(getTileNumber() * sizeof(Tile));
    m_updateBuff.reserve(getTileNumber());

    m_img = GenImageColor(m_gridWidth, m_gridWidth, WHITE);
    m_tex = LoadTextureFromImage(m_img);
    SetTextureFilter(m_tex, TEXTURE_FILTER_POINT);
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
    MemFree(encoded);
    MemFree(compressed);
}

void simu::from_json(const json & json, Grid & grid)
{
    // TODO: Décompresser les données
    // TODO: Ajouter les phéromones dans m_updateBuffer
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
