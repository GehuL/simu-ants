#include "tiles.h"
#include "utils.h"

using namespace simu;

// On utilise le système d'allocation de raylib pour cette class! 


Grid::Grid(const int tileSize) : m_gridWidth(0), m_tileSize(tileSize)
{

}

Grid::~Grid()
{
    if(m_grid != NULL)
        MemFree(m_grid);
}

void Grid::unload()
{
    if(m_grid != NULL)
    {
        MemFree(m_grid);
        m_grid = NULL;
    }
    
    m_updateBuff.clear();

    UnloadImage(m_img);
    UnloadTexture(m_tex);
}

void Grid::draw()
{
    if(m_gridWidth <= 0)
        return;

    // TODO: Utiliser UpdateTextureRec pour mettre à jour que certains pixels
    UpdateTexture(m_tex, m_img.data);

    const int gridWidthPixel = getTileSize() * m_gridWidth;
    DrawTexturePro(m_tex, (Rectangle) {0, 0, (float) m_tex.width, (float) m_tex.width}, 
                        (Rectangle) {0, 0, (float) gridWidthPixel, (float) gridWidthPixel}, 
                        (Vector2){0, 0}, 0, WHITE);

    //Draw vertical lines
    for(int i = 0; i < m_gridWidth; i++)
    {
        DrawLine(i*m_tileSize, 0, i*m_tileSize, m_tileSize*m_gridWidth, GRID_COLOR);
        DrawLine(0, i*m_tileSize, m_tileSize*m_gridWidth, i*m_tileSize, GRID_COLOR);
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

void Grid::init(int gridWidth)
{
    if(gridWidth <= 0)
        return;

    unload();

    m_gridWidth = gridWidth;

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
    json["width"] = grid.getGridWidth();

    std::string data;
    compressGrid(grid, data);
    json["data"] = data;
}

void simu::from_json(const json & json, Grid & grid)
{
    // TODO: Ajouter les phéromones dans m_updateBuffer
    auto rowdata = json.at("data");
    
    if(!rowdata.is_string())
        throw std::runtime_error("Impossible de charger la grille: La grille est corrompu");
    
    std::string data = rowdata.template get<std::string>();
    decompressGrid(grid, data, json.at("width"));
}

void simu::compressGrid(const Grid& grid, std::string& output)
{
    int compressed_len = 0, encoded_len = 0;
    unsigned char* compressed = CompressData((const unsigned char*)(grid.m_grid), grid.getTileNumber() * sizeof(Tile), &compressed_len);
    char* encoded = EncodeDataBase64((const unsigned char*)(compressed), compressed_len, &encoded_len);
    encoded = (char*) MemRealloc((void*) (encoded), encoded_len + 1);
    encoded[encoded_len] = '\0';

    output = encoded;

    MemFree(encoded);
    MemFree(compressed);
}

void simu::decompressGrid(Grid& grid, std::string &data, int gridWidth)
{
    int decoded_len = 0, decompressed_len = 0;
    unsigned char* decoded = DecodeDataBase64(reinterpret_cast<const unsigned char*>(data.c_str()), &decoded_len);
    unsigned char* decompressed = DecompressData(decoded, decoded_len, &decompressed_len);
    
    int tilesNumber = decompressed_len / sizeof(Tile);
    if(gridWidth*gridWidth != tilesNumber) // Test que la grille est bien carré
        throw std::runtime_error("Impossible de charger la grille: La grille est corrompu");
    
    if(decoded)
        MemFree(decoded);

    grid.unload();

    grid.m_gridWidth = gridWidth;
    grid.m_grid = reinterpret_cast<Tile*>(decompressed);

    grid.m_img = GenImageColor(grid.m_gridWidth, grid.m_gridWidth, WHITE);
    grid.m_tex = LoadTextureFromImage(grid.m_img);
    SetTextureFilter(grid.m_tex, TEXTURE_FILTER_POINT);

    // Update l'image et les phéromones
    for(int index = 0; index < grid.m_gridWidth*grid.m_gridWidth; index++)
    {
        const Tile tile = grid.m_grid[index];
        grid.setTile(tile, index);
        if(tile.type == Type::PHEROMONE)
        {
            grid.m_updateBuff.push_back(index);
        }
    }

}

void Grid::fromImage(const std::string& file)
{
    Image image = LoadImage(file.c_str());

    if(image.data == NULL)
        throw std::runtime_error("Impossible de charger le fichier");
    else if(image.width != image.height)
        throw std::runtime_error("Width and height must be equals");

    unload();

    m_img = image;
    m_tex = LoadTextureFromImage(m_img);
    SetTextureFilter(m_tex, TEXTURE_FILTER_POINT);
    
    m_gridWidth = m_img.width;
    m_grid = (Tile*) MemAlloc(sizeof(Tile) * getTileNumber());

    for(int y = 0; y < m_img.height; y++)
    {
        for(int x = 0; x < m_img.width; x++)
        {
            Color color = GetImageColor(m_img, x, y);
            Tile tile = fromColor(color);
            setTile<false>(tile, x, y);
        }
    }
}

Tile simu::fromColor(const Color& color)
{
    std::array<Tile, 4> tiles {AIR, GROUND, FOOD, WALL};
    return *std::find_if(tiles.begin(), tiles.end(), [color](auto t) { return t.color == color;});
}

bool simu::operator==(const Color &c1, const Color &c2)
{
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}
