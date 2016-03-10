/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Title Screen Functions Definitions (Init, Update, Draw, Unload)
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

// Title screen global variables
static int framesCounter;
static int finishScreen;

static int titleTextureScale;
static Texture2D titleTexture;

static Texture2D bgTexture;

static char *playMessage; 
int playMessageFontSize;

static float playMessageAlpha;
static int playMessageBlinkCounter;
static int playMessageBlinkDuration;
static float playMessageAlphaIncrement;
static bool isPlayMessageFadingIn;

static bool showCredits;

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void InitTitleScreen(void)
{
    // Initialize TITLE screen
    framesCounter = 0;
    finishScreen = 0;
    
    // Title texture init.
    titleTextureScale = 10;
    titleTexture = LoadTexture("assets/title/TapToJAmp_Title.png");
    
    bgTexture = LoadTexture("assets/title/bg_main.png");
    
    // Play message init.
    playMessage = "PRESS SPACE to JAMP!";
    playMessageFontSize = 32;
    
    playMessageAlpha = 0;
    playMessageBlinkCounter = 0;
    playMessageBlinkDuration = 8; // Fade duration (frames)
    playMessageAlphaIncrement = (float)1/playMessageBlinkDuration;
    isPlayMessageFadingIn = true;
    
    showCredits = false;
}

// Title Screen Update logic
void UpdateTitleScreen(void)
{
    // Update TITLE screen
    
    if (IsKeyPressed('C')) showCredits = !showCredits;

    // Press SPACE to change to GAMEPLAY screen
    if (IsKeyPressed(KEY_SPACE))
    {
        //finishScreen = 1;   // OPTIONS
        finishScreen = 2;   // GAMEPLAY
    }
    
    // Play message blinking logic
    if (playMessageBlinkCounter<playMessageBlinkDuration)
    {
        if (isPlayMessageFadingIn) 
        {
            playMessageAlpha += playMessageAlphaIncrement;
            playMessageBlinkCounter++;
        }
        else 
        {
            // Fade out is twice fast
            playMessageAlpha -= playMessageAlphaIncrement*2;
            playMessageBlinkCounter += 1*2;
        }   
    }
    else
    {
        if (isPlayMessageFadingIn) playMessageAlpha = 1;
        else playMessageAlpha = 0;
        
        isPlayMessageFadingIn = !isPlayMessageFadingIn;
        playMessageBlinkCounter = 0;  
    }
}

// Title Screen Draw logic
void DrawTitleScreen(void)
{
    // Draw TITLE screen
    //DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), SKYBLUE);
    DrawTextureEx(bgTexture, (Vector2){0, 0}, 0, 8, WHITE);
    
    DrawTextureEx(titleTexture, (Vector2){GetScreenWidth()/2-(titleTexture.width*titleTextureScale)/2, GetScreenHeight()/2-(titleTexture.height*titleTextureScale)/2-50}, 
    0, titleTextureScale, WHITE);
    
    DrawText(playMessage, GetScreenWidth()/2-MeasureText(playMessage, playMessageFontSize)/2, GetScreenHeight()-100, playMessageFontSize, Fade(YELLOW, playMessageAlpha));
    
    DrawText("Press <P> in game for volume settings", 10, GetScreenHeight() - 22, 20, WHITE);
    
    DrawText("Copyright (c) 2016 Marc Montagut", GetScreenWidth()/2 + 105, GetScreenHeight() / 2 + 5, 20, WHITE);
}

// Title Screen Unload logic
void UnloadTitleScreen(void)
{
    // Unload TITLE screen
    UnloadTexture(titleTexture);
    UnloadTexture(bgTexture);
}

// Title Screen should finish?
int FinishTitleScreen(void)
{
    //StopMusicStream();
    return finishScreen;
}