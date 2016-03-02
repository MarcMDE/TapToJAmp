/**********************************************************************************************
*   
*   Tap To JAmp - gameplay_screen
*
*   Tap To JAmp is a "The Impossible Game" / "Geometry Dash" clone developed by Marc Montagut (@MarcMDE)
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
#include "satcollision.h"
#include "c2dmath.h"
#include <stdio.h>
#include <stdlib.h>

#define GAME_SPEED 60

#define CELL_SIZE 48
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

typedef struct BoxCollider
{
    SATBox box;
    bool isActive;
}BoxCollider;

typedef struct TriCollider
{
    SATTri tri;
    bool isActive;
}TriCollider;

typedef struct Player
{
    Transform2D transform;
    DynamicObject dynamic;
    BoxCollider collider;
    Easing rotationEasing;
    bool isAlive;
    Texture2D texture;
    Color color;
}Player;

typedef struct ObjectStates
{
    bool isActive;
    bool isInScreen;
    bool isUp;
}ObjectStates;

typedef struct BoxGameObject
{
    Transform2D transform;
    BoxCollider collider;
    ObjectStates state;
}BoxGameObject;

typedef struct TriGameObject
{
    Transform2D transform;
    TriCollider collider;
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

bool isGameplayStopped;

Camera2D camera;

Player player;

TriGameObject tris[3];
Vector2 trisSourcePosition[3];
Vector2 triNormals[3];
Texture2D trisTexture;

BoxGameObject platf[3];
Vector2 platfSourcePosition[];
Vector2 platfNormals[2]:
Texture2D platfTexture;

bool deadCounter;
bool deadSpan;

//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
Vector2 GetOnGridPosition(Vector2 coordinates);
void InitPlayer(Player *p, Vector2 coordinates, Vector2 speed, float rotationSpan);
void UpdatePlayer(Player *p);
void SetPlayerAsGrounded(Player *p, int landPositionY);
void DrawPlayer (Player p);
void SetOnCameraPosition (Vector2 *position, Vector2 sourcePosition, Camera2D camera);
void UpdateOnCameraGameObject (Vector2 *position, ObjectStates *state, Vector2 sourcePosition, Camera2D camera);
void UpdateTris (TriGameObject *tris, Vector2 *sourcePosition, Vector2 playerPosition, Camera2D camera);
void CheckPlayerTrisCollision (Player *p, TriGameObject *tris);
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    
    gridLenght.x = GetScreenWidth()/CELL_SIZE;
    gridLenght.y = GetScreenHeight()/CELL_SIZE;
    
    // Set ground position in the bottom of the desired cell. 
    groundY = GetOnGridPosition((Vector2){0, gridLenght.y-2}).y + CELL_SIZE/2;
    
    // Set gravity
    gravity.direction = (Vector2){0, 1};
    gravity.value = 0.95f;
    gravity.force = Vector2FloatProduct(gravity.direction, gravity.value);
    
    // Set camera
    camera.position = Vector2Zero();
    camera.direction = Vector2Right();
    camera.speed = (Vector2){5.2f, 0};
    camera.isMoving = false;
    
    isGameplayStopped = true;
    
    // Counter on player dead (before level reset)
    deadCounter = 0;
    deadSpan = 2 * GAME_SPEED;
    
    InitPlayer(&player, (Vector2){5, gridLenght.y-2}, (Vector2){0, 13.5f}, 0.75f * GAME_SPEED);
    
    // Init Triangles
    trisTexture = LoadTexture("assets/gameplay/tri_main.png");
    
    for (int i=0; i<3; i++)
    {
        tris[i].transform.position = GetOnGridPosition((Vector2) {20 + i * 5, gridLenght.y-2});
        tris[i].transform.scale = 1;
        tris[i].transform.rotation = 0;
        trisSourcePosition[i] = tris[i].transform.position;
        
        InitSATTri(&tris[i].collider.tri, tris[i].transform.position, (Vector2){trisTexture.width, trisTexture.height}, tris[i].transform.rotation);
        tris[i].collider.isActive = false;
    
        tris[i].state.isActive = true;
        tris[i].state.isInScreen = false;
        tris[i].state.isUp = true;
    }
    
    SetNormals(tris[0].collider.tri.points, triNormals, 3, true);
    
    // Init Platforms
    platfTexture = LoadTexture("assets/gameplay/platf_main.png");
    
    for (int i=0; i<3; i++)
    {
        platf[i].transform.position = GetOnGridPosition((Vector2) {36 + i, gridLenght.y-2});
        platf[i].transform.scale = 1;
        platf[i].transform.rotation = 0;
        platfSourcePosition[i] = tris[i].transform.position;
        
        InitSATBox(&platf[i].collider.box, platf[i].transform.position, (Vector2){platfTexture.width, platfTexture.height}, platf[i].transform.rotation);
        platf[i].collider.isActive = false;
    
        platf[i].state.isActive = true;
        platf[i].state.isInScreen = false;
        platf[i].state.isUp = true;
    }
    
    // Set AACube normals (Right/Left + Up/Down)
    platfNormals[0] = Vector2Up();
    platfNormals[1] = Vecto2Right();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    if (!isGameplayStopped)
    {
        camera.position = Vector2Add(camera.position, Vector2Product(camera.direction, camera.speed));
        printf("%f\n", camera.position.x);
        
        // TODO: Check if player landed (or collided) on a platform.
    
        // Check if player collided with a triangle
        CheckPlayerTrisCollision(&player, tris);
        
        // Update game objects position after checking the collisions, so the player will see the collision drawed (otherwise it could be skiped)
        UpdateTris(tris, trisSourcePosition, player.transform.position, camera);
        UpdatePlayer(&player);   
    }
    else
    {
        if(IsKeyPressed(KEY_SPACE))
        {
            // Start gameplay (first time or after player dies)
            isGameplayStopped = false;
            camera.isMoving = true;
        }
    }

    // Press enter to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER))
    {
        finishScreen = 1;
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // Draw Ground
    DrawRectangle(0, groundY, GetScreenWidth(), 2, BLACK);
    
    // Draw Game Grid
    for (int i=0; i<gridLenght.x+1; i++) DrawRectangle(i*CELL_SIZE, 0, 1, GetScreenHeight(), LIGHTGRAY); // Columns
    for (int i=0; i<gridLenght.y; i++) DrawRectangle(0, i*CELL_SIZE, GetScreenWidth(), 1, LIGHTGRAY); // Rows

    DrawText("GAMEPLAY SCREEN", 20, 20, 40, MAROON);
    //DrawText("PRESS ENTER to JUMP to ENDING SCREEN", 170, 220, 20, MAROON);
    
    // Draw Tris
    for (int i=0; i<3; i++)
    {
        DrawTexturePro(trisTexture, (Rectangle){0, 0, trisTexture.width, trisTexture.height}, (Rectangle){tris[i].transform.position.x, tris[i].transform.position.y, 
        trisTexture.width, trisTexture.height}, (Vector2){trisTexture.width/2, trisTexture.height/2}, tris[i].transform.rotation, WHITE);
    }
    
    DrawPlayer(player);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    UnloadTexture(player.texture);
    UnloadTexture(trisTexture);
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
    position.x = coordinates.x + CELL_SIZE/2;
    position.y = coordinates.y + CELL_SIZE/2;
    return position; 
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
    p->dynamic.isGrounded = false;
    
    //Set player texture
    p->texture = LoadTexture("assets/gameplay/character/main_cube.png");
    
    // Set p color
    p->color = WHITE;
    
    // Init p boxCollider
    InitSATBox(&p->collider.box, p->transform.position, (Vector2){p->texture.width, p->texture.height}, p->transform.rotation);
    
    // Init easing
    p->rotationEasing.t = 0;
    p->rotationEasing.b = 0;
    p->rotationEasing.c = 180; // Try with 90
    p->rotationEasing.d = rotationSpan;
    p->rotationEasing.isFinished = false;
    
    p->isAlive = true;
}

void UpdatePlayer(Player *p)
{   
    // TODO: Check collisions order (avoid player-ground and player-platform overlaping)
    
    // Check if player landed on the ground
    if (p->transform.position.y + p->collider.box.size.y/2 >= groundY) SetPlayerAsGrounded(p, groundY);

    if (!p->dynamic.isGrounded)
    {
        // Add gravity force
        p->dynamic.velocity = Vector2Add(p->dynamic.velocity, gravity.force);
    }
    else
    {
        // Check jump key
        if (IsKeyPressed(KEY_SPACE)) 
        {
            p->dynamic.isGrounded = false;
            
            // Apply jump force
            p->dynamic.velocity = Vector2Product(p->dynamic.direction, p->dynamic.speed);
        }
        
        // TODO: Init easing
    }
    
    // Update player transform
    p->transform.position = Vector2Add(p->transform.position, p->dynamic.velocity);
    
    // Set position again (in case player had collided with the ground in this update)
    if (p->transform.position.y + p->collider.box.size.y/2 >= groundY) SetPlayerAsGrounded(p, groundY);
    
    // Update player collider
    UpdateSATBox(&p->collider.box, p->transform.position, Vector2FloatProduct(p->collider.box.size, p->transform.scale), p->transform.rotation);
    
    // Set player isGrounded to false since it has to be checked every frame (player could be falling from a platform)
    p->dynamic.isGrounded = false; 
}

void SetPlayerAsGrounded(Player *p, int landPositionY)
{
    p->dynamic.isGrounded = true;
    p->dynamic.velocity.y = 0;
    p->transform.position.y = landPositionY - p->collider.box.size.y/2;
}

void DrawPlayer (Player p)
{
    DrawTexturePro(p.texture, (Rectangle){0, 0, p.texture.width, p.texture.height}, (Rectangle){p.transform.position.x, 
    p.transform.position.y, p.texture.width, p.texture.height}, (Vector2){p.texture.width/2, 
    p.texture.height/2}, p.transform.rotation, p.color);
}

void SetOnCameraPosition (Vector2 *position, Vector2 sourcePosition, Camera2D camera)
{
    *position = Vector2Sub(sourcePosition, camera.position);
}

void UpdateOnCameraGameObject (Vector2 *position, ObjectStates *state, Vector2 sourcePosition, Camera2D camera)
{
    SetOnCameraPosition(position, sourcePosition, camera);
    
    if (state->isInScreen)
    {
        // Set object un-active if leaves the screen (from left)
        if (position->x + CELL_SIZE/2 < 0) 
        {
            state->isInScreen = false;
            state->isActive = false;
        }
    }
    else
    {
        // Set object "drawable" if enters the screen (from right)
        if (position->x - CELL_SIZE/2 < GetScreenWidth())
        {
            state->isInScreen = true;
        }
    }
}

void UpdateTris (TriGameObject *tris, Vector2 *sourcePosition, Vector2 playerPosition, Camera2D camera)
{
    for (int i=0; i<3; i++)
    {
        if (tris[i].state.isActive)
        {
            UpdateOnCameraGameObject(&tris[i].transform.position, &tris[i].state, sourcePosition[i], camera);
            
            if (tris[i].state.isInScreen && tris[i].transform.position.x-CELL_SIZE*1.5f < playerPosition.x) // CELL_SIZE*1.5 is a safe value for the player-collider distance checking
            {
                // Tri is on the player "colision zone"
                tris[i].collider.isActive = true;
                
                // TODO: Replace to Updat SATAATri (since triangles won't change its rotation/scale atm)
                UpdateSATTri(&tris[i].collider.tri, tris[i].transform.position, Vector2FloatProduct(tris[i].collider.tri.size, tris[i].transform.scale), tris[i].transform.rotation);
            }
            else if (tris[i].state.isInScreen && tris[i].transform.position.x + CELL_SIZE*1.5f < player.transform.position.x) // CELL_SIZE*1.5 is a safe value for the player-collider distance checking
            {
                // Tri is out the player "colision zone"
                tris[i].collider.isActive = false;
            }
        }
    }
}

void CheckPlayerTrisCollision (Player *p, TriGameObject *tris)
{
    for (int i=0; i<3; i++)
    {
        if (tris[i].collider.isActive)
        {
            if (SATPolyPolyNCollide(p->collider.box.points, 4, tris[i].collider.tri.points, triNormals, 3))
            {
                // Player collided with a triangle
                // Set player as dead
                p->isAlive = false;
            }
        }
    }
}

// TODO: Code platforms Update