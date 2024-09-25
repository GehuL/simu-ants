/*******************************************************************************************
*
*   raylib [core] example - Basic 3d example
*
*   Welcome to raylib!
*
*   To compile example, just press F5.
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2013-2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/
#include "config.h"

#include "raylib.h"
#include "stdlib.h"
#include "utils.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <iostream>

#define MIN(a, b) a > b ? b : a

#define tilesNumber 100

#define T_PHEROMONE (Tile) {0, PINK, PHEROMONE, 0}
#define T_AIR (Tile) {0, WHITE, AIR, 0}
#define T_WALL (Tile) {1, BROWN, WALL, 0}

typedef enum Type
{
    AIR,
    WALL,
    ANT,
    PHEROMONE,
    FOOD
} Type;

typedef struct Tile
{
    bool occuped;
    Color color;
    Type type;
    void* data;
} Tile;

typedef struct Ant
{
    float x, y;
    float angle;
    float targetAngle;
    int rotateCd;
    
    int dirX, dirY; // Direction
} Ant;

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
const int screenWidth = 1000;
const int screenHeight = 1000;

const int tileSize = 10;

float tickPerSecond = 100;
float framePerSecond = 60;

float sliderTickValue;
float sliderFpsValue;

// Profiling
int lastTickCounter;
int lastFrameCounter;

bool noDelay = false; // Active un delai pour ne pas surcharger le processeur (limite le nombre de tick/s) 

Ant ants[8];
Tile tiles[tilesNumber][tilesNumber]; // 100*100*10 = 100 000 bytes = 0.1 Mbytes

Camera2D camera;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------

// LOGIC FUNCTIONS
static void InitGame(void);
static void UpdateGame(void);
static void UpdateTiles();
static void UpdateAnts(void);

static void MoveToward(Ant* ant, float speed);
static void SetTile(Tile tile, int x, int y);

static void HandleKeyboard(void);
static void HandleMouse(void);

// DRAW FUNCTIONS
static void DrawGame(void);
static void DrawTiles(void);
static void DrawAnts(void);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main2()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "raylib");
  
    // SetTargetFPS(60);               // Set our game to run at 60 frames-per-seconde
    // SetWindowState(FLAG_VSYNC_HINT);
    
    InitGame();

    double lastUpdateTime = 0;
    double lastDrawTime = 0;
    double lastProfilerTime = 0;

    int tickCounter = 0;
    int frameCounter = 0;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        const double drawTargetTime = (1.0 / static_cast<double>(framePerSecond));
        const double updateTargetTime = (1.0 / static_cast<double>(tickPerSecond));

        // Update logic
        double updateDelta = GetTime() - lastUpdateTime;
        if(updateDelta >= updateTargetTime)
        {
            double now = GetTime();
            lastUpdateTime = now;
            UpdateGame();
            tickCounter++;
        }

        // Draw frame
        double drawDelta = GetTime() - lastDrawTime;

        if(drawDelta >= drawTargetTime)
        {
            double updateDelta2 = GetTime() - lastUpdateTime;
            if(updateDelta2 >= updateTargetTime && !noDelay) // Pas le temps pour dessiner, priorité sur update
            {
                frameCounter--;
                TRACELOG(LOG_INFO, "Frame skipped !");
            }else
            {
                double now = GetTime();
                lastDrawTime = now;
                DrawGame();
                frameCounter++;
            }
        }

        // Profiling update
        double now = GetTime();
        double profilerDelta = now - lastProfilerTime;
        if(profilerDelta > 1.0)
        {
            lastProfilerTime = now; // pas besoin d'être précis pour le profiling
            
            lastFrameCounter = frameCounter;
            frameCounter = 0;

            lastTickCounter = tickCounter;
            tickCounter = 0;
        }
    
        // Différence de temps libre entre update et draw
        double waitTime = MIN(drawTargetTime - drawDelta, updateTargetTime - updateDelta);
        if(!noDelay && waitTime >= 0.0) // Il reste du temps pour mettre en pause  
        {
            // Désactiver le waitTime permet d'augmenter la priorité du processus 
            WaitTime(waitTime);
        }
    }


    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();                  // Close window and OpenGL context
    //-------------------------------------------------------------------------d-------------

    return 0;
}

static void UpdateGame(void)
{
    UpdateTiles();
    UpdateAnts();
}

// Update and draw game frame
static void DrawGame(void)
{
    BeginDrawing();

        ClearBackground(RAYWHITE);

        HandleKeyboard();
        HandleMouse();

        BeginMode2D(camera);

        DrawTiles();
        DrawAnts();

        EndMode2D();

        DrawText(TextFormat("%d FPS\n%d TPS", lastFrameCounter, lastTickCounter), 5, 0, 20, GREEN);

        GuiSlider((Rectangle){ screenWidth / 2 - 100, screenHeight - 20, 200, 16 }, "FPS 1", "100%", &framePerSecond, 1, 300);
        GuiSlider((Rectangle){ screenWidth / 2 - 100, screenHeight - 50, 200, 16 }, "TPS 0", "100%", &tickPerSecond, 0, 1000);
        
        if(tickPerSecond >= 1000)
        {
            noDelay = true;
            tickPerSecond = 999999999;
        }
        else
            noDelay = false;

    EndDrawing();

#ifdef SUPPORT_CUSTOM_FRAME_CONTROL
    SwapScreenBuffer();
    PollInputEvents();
#endif
}

static void InitGame(void)
{
    camera.offset = (Vector2){ 0, 0 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    for(size_t i = 0; i < sizeof(tiles) / sizeof(Tile); i++)
        tiles[0][i] = T_AIR;

    for(size_t i = 0; i < sizeof(ants) / sizeof(Ant); i++)
    {
        ants[i].x = 40;
        ants[i].y = 40;
        ants[i].angle = 45*i;
    }
}

static void HandleKeyboard(void)
{
    constexpr float camera_speed = 3.5f; 

    if (IsKeyDown(KEY_DOWN)) camera.offset.y -= camera_speed;
    if (IsKeyDown(KEY_UP)) camera.offset.y += camera_speed;
    if (IsKeyDown(KEY_RIGHT)) camera.offset.x -= camera_speed;
    if (IsKeyDown(KEY_LEFT)) camera.offset.x += camera_speed;

    if(IsKeyPressed(KEY_ENTER)) InitGame();
}

static void DrawTiles(void)
{
    for(int i = 0; i < tilesNumber; i++)
    {
        for(int k = 0; k < tilesNumber; k++)
        {
            DrawRectangle(k*tileSize, i*tileSize, tileSize, tileSize, tiles[i][k].color);
        }
    }
}

static void DrawAnts(void)
{
    for(int i = 0; i < sizeof(ants) / sizeof(Ant); i++)
    {
        Ant* ant = &ants[i];
        DrawRectangle(ant->x*tileSize, ant->y*tileSize, tileSize, tileSize, DARKBROWN);
    }
}

static Tile GetTile(int x, int y)
{
    if(x < 0 || x > tilesNumber - 1 || y < 0 || y > tilesNumber - 1)
        return T_WALL;

    return tiles[y][x];
}

static void UpdateAnts(void)
{
    for(int i = 0; i < sizeof(ants) / sizeof(Ant); i++)
    {
        Ant* ant = &ants[i];

        Tile tile = GetTile((int) ant->x + ant->dirX, (int) ant->y + ant->dirY);

        if(tile.type == WALL)
        {
            ant->angle += 180;
            ant->targetAngle = ant->angle;
        }else
        {
            if(--ant->rotateCd <= 0)
            {
                ant->rotateCd = GetRandomValue(40, 180);
                ant->targetAngle = GetRandomValue(0, 360);
            }

            int da = ant->targetAngle - ant->angle;
            ant->angle += da * 0.01f;

            if(ant->angle < 0)
                ant->angle = 360 - ant->angle;
        }
        MoveToward(ant, 0.1f);

        SetTile(T_PHEROMONE, ant->x, ant->y);
    }      
}

static void SetTile(Tile tile, int x, int y)
{
    if(x < 0 || x > tilesNumber - 1 || y < 0 || y > tilesNumber - 1)
        return;

    tiles[y][x] = tile;
}


static void UpdateTiles()
{
    for(int y = 0; y < tilesNumber; y++)
    {
        for(int x = 0; x < tilesNumber; x++)
        {
            Tile* tile = &tiles[y][x];

            switch(tile->type)
            {
                case ANT:
                break;
                case WALL:
                break;
                case FOOD:
                break;
                case AIR:
                break;
                case PHEROMONE:
                    if(tile->color.a > 0)
                        tile->color.a -= 0.1f;
                    else
                        SetTile(T_AIR, x, y);
                break;
            }
        }
    }

}

static void MoveToward(Ant* ant, float speed)
{
    if(ant == 0)
        return;

    int dir = ((int) ant->angle % 360) / 45;

    int dirX[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dirY[] = {0, 1, 1, 1, 0, -1, -1, -1};

    ant->x += dirX[dir] * speed;
    ant->y += dirY[dir] * speed;

    ant->dirX = dirX[dir];
    ant->dirY = dirY[dir];
}

/* static void MoveTile(int fromX, int fromY, int toX, int toY)
{
    Tile* from = &tiles[fromY][fromX];
    Tile* to = &tiles[toY][toX];

    *to = *from;

    from->occuped = false;
    from->data = 0;
    from->type = AIR;
    from->color = WHITE;
} */

static void HandleMouse(void)
{
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 pos = GetScreenToWorld2D(GetMousePosition(), camera);
        
        int tileX = pos.x / tileSize;
        int tileY = pos.y / tileSize;
        
        // Check grid bounds 
        if(tileX >= 0 && tileX < tilesNumber && tileY >= 0 && tileY < tilesNumber)
        {
            // Swap to RED or WHITE
            SetTile(T_WALL, tileX, tileY);
        }
    }else if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        Vector2 pos = GetScreenToWorld2D(GetMousePosition(), camera);

        int tileX = pos.x / tileSize;
        int tileY = pos.y / tileSize;

        // Check grid bounds 
        if(tileX >= 0 && tileX < tilesNumber && tileY >= 0 && tileY < tilesNumber)
        {
            // Swap to RED or WHITE
            SetTile(T_AIR, tileX, tileY);
        }
    }

    camera.zoom += GetMouseWheelMove() * 0.1f;
}
