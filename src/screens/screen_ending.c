/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Ending Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014 Ramon Santamaria (Ray San - raysan@raysanweb.com)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"

#define MAX_CUBES 150
#define CELL_SIZE 48

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------

// Ending screen global variables
static int framesCounter;
static int finishScreen;

static int cubesAmount;
static int counterMult; 

static Texture2D victoryTexture;
static int victoryTextureScale;

static Texture2D cube;
static Vector2 cubesPosition[MAX_CUBES];
float cubesRotation[MAX_CUBES];
float cubesScale[MAX_CUBES];

static char playAgainText[30] = "Press ENTER to play again";
static Rectangle playAgainBg;
static int playAgainTextSize;

//----------------------------------------------------------------------------------
// Ending Screen Functions Definition
//----------------------------------------------------------------------------------

// Ending Screen Initialization logic
void InitEndingScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    
    counterMult = 1;
    cubesAmount = 0;
    cube = LoadTexture("assets/gameplay/character/main_cube.png");
    
    victoryTexture = LoadTexture("assets/ending/victory_main.png");
    victoryTextureScale = 10;

    playAgainTextSize = 60;
    playAgainBg = (Rectangle){GetScreenWidth()/2 - MeasureText(playAgainText, playAgainTextSize) / 2 - 2, GetScreenHeight() - 80 - 2, 
    MeasureText(playAgainText, playAgainTextSize) + 4, playAgainTextSize + 4};
}

// Ending Screen Update logic
void UpdateEndingScreen(void)
{   
    if (cubesAmount < MAX_CUBES)
    {
        if (framesCounter > 60)
        {
            cubesPosition[cubesAmount] = (Vector2){GetRandomValue(0, GetScreenWidth()), GetRandomValue(0, GetScreenHeight())};
            cubesRotation[cubesAmount] = GetRandomValue(0, 360);
            cubesScale[cubesAmount] = GetRandomValue(1, 4);
            
            framesCounter = 0;
            cubesAmount++;
            counterMult++;
        }
        
        framesCounter += counterMult;
    }   
    
    // Press enter to return to TITLE screen
    if (IsKeyPressed(KEY_ENTER))
    {
        finishScreen = 1;
    }
}

// Ending Screen Draw logic
void DrawEndingScreen(void)
{
    // TODO: Draw ENDING screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BEIGE);
    
    for (int i=0; i<cubesAmount; i++)
    {
        DrawTexturePro(cube, (Rectangle){0, 0, CELL_SIZE, CELL_SIZE}, (Rectangle){cubesPosition[i].x, 
        cubesPosition[i].y, CELL_SIZE * cubesScale[i], CELL_SIZE * cubesScale[i]}, (Vector2){(CELL_SIZE * cubesScale[i]) / 2, 
        (CELL_SIZE * cubesScale[i]) / 2}, -cubesRotation[i], WHITE);   
    }
    
    DrawTextureEx(victoryTexture, (Vector2){GetScreenWidth()/2-(victoryTexture.width*victoryTextureScale)/2, GetScreenHeight()/2-(victoryTexture.height*victoryTextureScale)/2-50}, 
    0, victoryTextureScale, WHITE);
    
    //DrawText("VICTORY!", GetScreenWidth()/2 - 120, 20, 50, WHITE);
    if (cubesAmount >= MAX_CUBES)
    {
        DrawRectangleRec(playAgainBg, BLACK);
        DrawText(playAgainText, playAgainBg.x + 2, playAgainBg.y + 2, playAgainTextSize, WHITE);        
    }
    
    DrawRectangle(5, 5, 120, 30, BLACK);
}

// Ending Screen Unload logic
void UnloadEndingScreen(void)
{
    UnloadTexture(cube);
}

// Ending Screen should finish?
int FinishEndingScreen(void)
{
    return finishScreen;
}