/**********************************************************************************************
*   
*   Tap To JAmp - gameplay_screen
*
*   Tap To JAmp is a The Impossible Game / Geometry Dash clone developed by Marc Montagut (@MarcMDE)
*   
-----------------------------------------------------------------------------------------------
*
*   raylib - Advance Game template
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
#include "satcollisions.h"
#include "c2dmath.h"
#include <stdio.h>
#include <stdlib.h>

#define GAME_SPEED 60

#define CELL_SIZE 32
#define ASSETS_SCALE 1

//----------------------------------------------------------------------------------
// Structs Definition (local to this module)
//----------------------------------------------------------------------------------

typedef struct Transform2D
{
    Vector2 position;
    float rotation;
    float scale;
} Transform2D;

typedef struct Camera2D
{
    Vector2 position;
    Vector2 direction;
    Vector2 speed;
    bool isMoving;
} Camera2D;

typedef struct Easing
{
    float t, b, c, d;
    bool isFinished;
}Easing;

typedef struct DynamicObject
{
    //Rectangle checker;
    Vector2 direction;
    Vector2 speed;
    Vector2 velocity;
    bool isGrounded;
    //int onAirCounter;
}DynamicObject;

typedef struct Player
{
    Transform2D transform;
    DynamicObject dynamic;
    SATBox boxCollider;
    Easing rotationEasing;
    bool isAlive;
    Texture2D texture;
    Color color;
}Player;

typedef struct ObjectStates
{
    bool isActive;
    bool isDrawed;
    bool isUp;
}ObjectStates;

typedef struct BoxGameObject
{
    Transform2D transform;
    SATBox boxCollider;
    ObjectStates state;
}BoxGameObject;

typedef struct TriGameObject
{
    Transform2D transform;
    SATTri triCollider;
    ObjectStates state;
}TriGameObject;

typedef struct GravityForce
{
    Vector2 direction;
    float value;
    Vector2 force;
}GravityForce;
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------

// Gameplay screen global variables
static int framesCounter;
static int finishScreen;

Vector2 gridLenght;
int groundY;
GravityForce gravity;

Player player;
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
Vector2 GetOnGridPosition(Vector2 coordinates)
void InitPlayer(Player *p, Vector2 coordinates, Vector2 speed, float rotationSpan);
void SetPlayerAsGrounded(Player *p, int landPositionY)
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    
    gridLenght.x = GetScreenWidth()/CELL_SIZE;
    gridLenght.y = GetScreenHeight()/CELL_SIZE;
    
    InitPlayer(&player, (Vector2){6, gridLenght.y-3}, (Vector2){0, 5}, 0.75f);
    
    // Set ground position in the bottom of the desired cell. 
    groundY = GetOnGridPosition((Vector2){0, gridLenght.y-3}).y + CELL_SIZE/2;
    
    // Set gravity
    gravity.direction = (Vector2){0, 1};
    gravity.value = 4;
    gravity.force = Vector2FloatProduct(gravity.direction, value);
    
    
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // TODO: Update GAMEPLAY screen variables here!

    // Press enter to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER))
    {
        finishScreen = 1;
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
    DrawText("GAMEPLAY SCREEN", 20, 20, 40, MAROON);
    DrawText("PRESS ENTER to JUMP to ENDING SCREEN", 170, 220, 20, MAROON);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}

// Returns the position (cell center) based on the coordinates over the current grid
Vector2 GetOnGridPosition(Vector2 coordinates)
{
    Vector2 position;
    Vector2Scale(&coordinates, CELL_SIZE);
    position.x = coordinates + CELL_SIZE/2;
    position.y = coordinates + CELL_SIZE/2;
    return coordinates; 
}

void InitPlayer(Player *p, Vector2 coordinates, Vector2 speed, float rotationSpan)
{
    // Init p transform
    p->transform.position = GetOnGridPosition(coordinates);
    p->transform.rotation = 0;
    p->transform.scale = 1;
    
    // Init p dynamic
    p->dynamic.direction = (Vector2){0, -1};
    p->dynamic.speed = speed;
    p->dynamic.velocity = Vector2Zero();
    p->isGrounded = false;
    
    // TODO: Set player texture
    //p->texture = LoadTexture("");
    
    // Set p color
    p->color = WHITE;
    
    // Init p boxCollider
    InitSATBox(&p->boxCollider, p->transform.position, (Vector2){p->texture.width, p->texture.height}, p->transform.rotation);
    
    // Init easing
    p->rotationEasing.t = 0;
    p->rotationEasing.b = 0;
    p->rotationEasing.c = 180; // Try with 90
    p->rotationEasing.d = rotationSpan;
    p->rotationEasing.isFinished = false;
}

void UpdatePlayer(Player *p)
{
    
    // Set player isGrounded to false since it has to be checked every frame (player would be falling from a platform)
    p->isGrounded = false; 
    
    // Check if player landed on the ground
    if (player->transform.position.y <= groundY) SetPlayerAsGrounded(&p, groundY)
    
    // TODO: Check if player landed (or collided) on a platform.
    
    // TODO: Check if player collided with a triangle

    if (!p->isGrounded)
    {
        // Add gravity force
        p->dynamic.velocity = Vector2Add(p->dynamic.velocity, gravity.force);
    }
    else
    {
        // Check jump key
        if (IsKeyPressed(KEY_SPACE)) 
        {
            p->isGrounded = false;
            
            // Apply jump force
            p->velocity = Vector2Product(p->dynamic.direction, p->dynamic.speed);
        }
        
        // TODO: Init easing
    }
    
    // TODO: Update player transform
    
    // TODO: Update player collider
}

void SetPlayerAsGrounded(Player *p, int landPositionY)
{
    p->dynamic.velocity.y = 0;
    p->transform.position.y = landPositionY - p->boxCollider.size.y/2;
}