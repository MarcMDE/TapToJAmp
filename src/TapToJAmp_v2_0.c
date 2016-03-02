/*******************************************************************************************
*
*   Tap To JAmp (v2.0) - DESKTOP - WINDOWS
*
*   Developed by Marc Montagut - @MarcMDE  
*
*   Copyright (c) 2016 Marc Montagut
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

*   raylib - Advance Game template
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (Ray San - raysan@raysanweb.com)
*
********************************************************************************************/

#include "raylib.h"
#include "screens/screens.h"    // NOTE: Defines global variable: currentScreen

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------

// Required variables to manage screen transitions (fade-in, fade-out)
float transAlpha = 0;
bool onTransition = false;
bool transFadeOut = false;
int transFromScreen = -1;
int transToScreen = -1;
float alphaDiff = 0.05f;
    
//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
void TransitionToScreen(int screen);
void UpdateTransition(void);
void DrawTransition(void);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
	// Initialization
	//---------------------------------------------------------
    const int screenWidth = 1024;
    const int screenHeight = 576;
	const char windowTitle[20] = "Tap To JAmp v2.0";
    
    InitWindow(screenWidth, screenHeight, windowTitle);

    // TODO: Load global data here (assets that must be available in all screens, i.e. fonts)
    
    // Setup and Init first screen
    /*
    InitLoadingScreen();
    currentScreen = LOADING;
    */
    // DEBUG
    InitTitleScreen();
    currentScreen = TITLE;
    
	SetTargetFPS(60);
	//----------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (!onTransition)
        {
            switch(currentScreen) 
            {                
                case LOADING:
                {
                    UpdateLoadingScreen();
                    
                    if (FinishLoadingScreen()) TransitionToScreen(LOGO);
                    
                } break;
                case LOGO: 
                {
                    UpdateLogoScreen();
                    
                    if (FinishLogoScreen()) TransitionToScreen(TITLE);
                    
                } break;
                case TITLE: 
                {
                    UpdateTitleScreen();
                    
                    if (FinishTitleScreen() == 1) TransitionToScreen(OPTIONS);
                    else if (FinishTitleScreen() == 2) TransitionToScreen(GAMEPLAY);

                } break;
                case OPTIONS:
                {
                    UpdateOptionsScreen();
                    
                    if (FinishOptionsScreen()) TransitionToScreen(TITLE);

                } break;
                case GAMEPLAY:
                { 
                    UpdateGameplayScreen();
                    
                    if (FinishGameplayScreen()) TransitionToScreen(ENDING);
  
                } break;
                case ENDING: 
                {
                    UpdateEndingScreen();
                    
                    if (FinishEndingScreen()) TransitionToScreen(TITLE);

                } break;
                default: break;
            }
        }
        else
        {
            // Update transition (fade-in, fade-out)
            UpdateTransition();
        }
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            switch(currentScreen) 
            {
                case LOADING: DrawLoadingScreen(); break;
                case LOGO: DrawLogoScreen(); break;
                case TITLE: DrawTitleScreen(); break;
                case OPTIONS: DrawOptionsScreen(); break;
                case GAMEPLAY: DrawGameplayScreen(); break;
                case ENDING: DrawEndingScreen(); break;
                default: break;
            }
            
            if (onTransition) DrawTransition();
        
            DrawFPS(screenWidth - 80, 5);
        
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    
    // TODO: Unload all global loaded data (i.e. fonts) here!
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
	
    return 0;
}

void TransitionToScreen(int screen)
{
    onTransition = true;
    transFromScreen = currentScreen;
    transToScreen = screen;
}

void UpdateTransition(void)
{
    if (!transFadeOut)
    {
        transAlpha += alphaDiff;

        if (transAlpha >= 1.0)
        {
            transAlpha = 1.0;
        
            switch (transFromScreen)
            {
                case LOADING: UnloadLoadingScreen(); break;
                case LOGO: UnloadLogoScreen(); break;
                case TITLE: UnloadTitleScreen(); break;
                case OPTIONS: UnloadOptionsScreen(); break;
                case GAMEPLAY: UnloadGameplayScreen(); break;
                case ENDING: UnloadEndingScreen(); break;
                default: break;
            }
            
            switch (transToScreen)
            {
                case LOADING: 
                {
                    InitLoadingScreen();
                    currentScreen = LOADING;
                } break;
                case LOGO:
                {
                    InitLogoScreen(); 
                    currentScreen = LOGO; 
                } break;
                case TITLE: 
                {
                    InitTitleScreen();
                    currentScreen = TITLE;                  
                } break;
                case OPTIONS:
                {
                    InitOptionsScreen(); 
                    currentScreen = OPTIONS;
                } break;
                case GAMEPLAY:
                {
                    InitGameplayScreen(); 
                    currentScreen = GAMEPLAY;
                } break;
                case ENDING:
                {
                    InitEndingScreen(); 
                    currentScreen = ENDING;
                } break;
                default: break;
            }
            
            transFadeOut = true;
        }
    }
    else  // Transition fade out logic
    {
        transAlpha -= alphaDiff;
        
        if (transAlpha <= 0)
        {
            transAlpha = 0;
            transFadeOut = false;
            onTransition = false;
            transFromScreen = -1;
            transToScreen = -1;
        }
    }
}

void DrawTransition(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, transAlpha));
}