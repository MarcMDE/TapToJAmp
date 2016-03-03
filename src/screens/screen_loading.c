/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Loading Screen Functions Definitions (Init, Update, Draw, Unload)
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

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------

// Logo screen global variables
static int framesCounter;
static int finishScreen;

//----------------------------------------------------------------------------------
// Loading Screen Functions Definition
//----------------------------------------------------------------------------------

// Loading Screen Initialization logic
void InitLoadingScreen(void)
{
    // TODO: Initialize LOADING screen variables here!
    framesCounter = 0;
    finishScreen = 0;
}

// Loading Screen Update logic
void UpdateLoadingScreen(void)
{
    // TODO: Update LOADING screen variables here!

    framesCounter++;    // Count frames

    // Wait for 1.5 seconds (90 frames) before jumping to LOGO screen
    if (framesCounter > 30)
    {
        finishScreen = true;
    }
}

// Loading Screen Draw logic
void DrawLoadingScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    DrawText("LOADING", 20, 20, 40, LIGHTGRAY);
}

// Loading Screen Unload logic
void UnloadLoadingScreen(void)
{
    // TODO: Unload LOADING screen variables here!
}

// Loading Screen should finish?
int FinishLoadingScreen(void)
{
    return finishScreen;
}