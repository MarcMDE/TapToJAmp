/**********************************************************************************************
*   
*   Tap To JAmp - gameplay_screen
*
*   Tap To JAmp is a "The Impossible Game" / "Geometry Dash" clone developed by Marc Montagut (@MarcMDE)
*
*   // TODO: Copyright
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
#include "ceasings.h"
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
    float t, b;
    float c[2];
    float d[2];
    bool isFinished;
}Easing;

typedef struct DynamicObject
{
    Vector2 prevPosition; // Previous frame position
    Vector2 direction;
    Vector2 speed;
    Vector2 velocity;
    bool isGrounded;
    bool isJumping;
    bool isFalling;
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
GravityForce gravity[2];

bool isGameplayStopped;

Camera2D camera;

Player player;

TriGameObject tris[3];
Vector2 trisSourcePosition[3];
Vector2 triNormals[3];
Texture2D trisTexture;

BoxGameObject platfs[3];
Vector2 platfsSourcePosition[3];
Vector2 platfNormals[2];
Texture2D platfsTexture;

bool deadCounter;
bool deadSpan;
float deadFadeAlpha;
bool deadFadeIn;
bool isDeadFadeFinished;

bool isGamePaused;
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
void UpdatePlatfs (BoxGameObject *platfs, Vector2 *sourcePosition, Vector2 playerPosition, Camera2D camera);
void CheckPlayerPlatfsCollision (Player *p, BoxGameObject *platfs);
void ResetGameplay ();
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
    gravity[0].direction = (Vector2){0, 1};
    gravity[0].value = 1;
    gravity[0].force = Vector2FloatProduct(gravity[0].direction, gravity[0].value);
    
    gravity[1].direction = (Vector2){0, 1};
    gravity[1].value = 2.5f;
    gravity[1].force = Vector2FloatProduct(gravity[1].direction, gravity[1].value);
    
    // Set camera
    camera.position = Vector2Zero();
    camera.direction = Vector2Right();
    camera.speed = (Vector2){5.45f, 0};
    camera.isMoving = false;
    
    isGameplayStopped = true;
    
    // Counter on player dead (before level reset)
    deadCounter = 0;
    deadSpan = 1.2f * GAME_SPEED;
    deadFadeAlpha = 0;
    deadFadeIn = true;
    isDeadFadeFinished = true;
    
    isGamePaused = false;
    
    InitPlayer(&player, (Vector2){5, gridLenght.y-2}, (Vector2){0, 14.5f}, 0.7f * GAME_SPEED);
    
    // Init Triangles
    trisTexture = LoadTexture("assets/gameplay/tri_main.png");
    
    for (int i=0; i<3; i++)
    {
        tris[i].transform.position = GetOnGridPosition((Vector2) {200 + i, gridLenght.y-2});
        if (i==2) tris[i].transform.position = GetOnGridPosition((Vector2) {250 + i * 5, gridLenght.y-2});
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
    
    // Init platfsorms
    platfsTexture = LoadTexture("assets/gameplay/platf_main.png");
    
    for (int i=0; i<3; i++)
    {
        platfs[i].transform.position = GetOnGridPosition((Vector2) {18 + i*20, gridLenght.y-2});
        platfs[i].transform.scale = 1;
        platfs[i].transform.rotation = 0;
        platfsSourcePosition[i] = platfs[i].transform.position;
        
        InitSATBox(&platfs[i].collider.box, platfs[i].transform.position, (Vector2){platfsTexture.width, platfsTexture.height}, platfs[i].transform.rotation);
        platfs[i].collider.isActive = false;
    
        platfs[i].state.isActive = true;
        platfs[i].state.isInScreen = false;
        platfs[i].state.isUp = true;
    }
    
    // Set AACube normals (Right/Left + Up/Down)
    platfNormals[0] = Vector2Up();
    platfNormals[1] = Vector2Right();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{   
    if (isDeadFadeFinished)
    {
        if (IsKeyPressed('P')) isGamePaused = !isGamePaused;
        if (IsKeyPressed('R')) isDeadFadeFinished = false;
        
        if (!isGamePaused)
        {
            if (!isGameplayStopped)
            {
                if (player.isAlive)
                {
                    camera.position = Vector2Add(camera.position, Vector2Product(camera.direction, camera.speed));
                    //printf("%f\n", camera.position.x);

                    // Update game objects position before checking the collisions, so the player will see the collision drawed (otherwise it could be skiped)
                    UpdateTris(tris, trisSourcePosition, player.transform.position, camera);
                    UpdatePlatfs (platfs, platfsSourcePosition, player.transform.position, camera); 
                    UpdatePlayer(&player);
                    
                    // Check if player landed on the ground
                    if (player.transform.position.y + player.collider.box.size.y/2 >= groundY) SetPlayerAsGrounded(&player, groundY);
                    // Check if player collided with a triangle
                    CheckPlayerTrisCollision(&player, tris);
                    // Check if player landed (or collided) on a platfsorm.
                    CheckPlayerPlatfsCollision(&player, platfs);
                }
                else
                {
                    if(deadCounter>=deadSpan)
                    {
                        isDeadFadeFinished = false;
                    }
                    else
                    {
                        deadCounter++;
                        // TODO: Add dead explosion effects (anim + sound)
                    }
                }
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
        }
    }
    else
    {
        ResetGameplay();
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
    
    // Draw Tris
    for (int i=0; i<3; i++)
    {
        DrawTexturePro(trisTexture, (Rectangle){0, 0, trisTexture.width, trisTexture.height}, (Rectangle){tris[i].transform.position.x, tris[i].transform.position.y, 
        trisTexture.width, trisTexture.height}, (Vector2){trisTexture.width/2, trisTexture.height/2}, tris[i].transform.rotation, WHITE);
        
        DrawTexturePro(platfsTexture, (Rectangle){0, 0, platfsTexture.width, platfsTexture.height}, (Rectangle){platfs[i].transform.position.x, platfs[i].transform.position.y, 
        platfsTexture.width, platfsTexture.height}, (Vector2){platfsTexture.width/2, platfsTexture.height/2}, platfs[i].transform.rotation, WHITE);
    }
    
    DrawPlayer(player);
    
    if (isGamePaused)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(LIGHTGRAY, 0.45f));
        DrawText("PAUSE", GetScreenWidth()/2 - 100, GetScreenHeight()/2-20, 40, DARKGRAY);
        DrawText("<P>", GetScreenWidth()/2 - 45, GetScreenHeight()/2 + 30, 24, DARKGRAY); 
    }
    
    if (!isDeadFadeFinished)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, deadFadeAlpha));
    }
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    UnloadTexture(player.texture);
    UnloadTexture(trisTexture);
    UnloadTexture(platfsTexture);
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
    p->dynamic.prevPosition = p->transform.position;
    p->dynamic.isGrounded = true;
    p->dynamic.isJumping = false;
    p->dynamic.isFalling = false;
    
    //Set player texture
    p->texture = LoadTexture("assets/gameplay/character/main_cube.png");
    
    // Set p color
    p->color = WHITE;
    
    // Init p boxCollider
    InitSATBox(&p->collider.box, p->transform.position, (Vector2){p->texture.width, p->texture.height}, p->transform.rotation);
    
    // Init easing
    p->rotationEasing.t = 0;
    p->rotationEasing.b = 0;
    p->rotationEasing.c[0] = 180;
    p->rotationEasing.c[1] = 90;
    p->rotationEasing.d[0] = rotationSpan;
    p->rotationEasing.d[1] = rotationSpan/2.5f;
    p->rotationEasing.isFinished = true;
    
    p->isAlive = true;
}

void UpdatePlayer(Player *p)
{   
    if (!p->dynamic.isGrounded)
    {   
        if (!p->dynamic.isFalling && !p->dynamic.isJumping)
        {
            // Player is falling from a platform
            p->rotationEasing.isFinished = false;
            p->dynamic.isFalling = true;
        }
        // Add gravity force
        p->dynamic.velocity = Vector2Add(p->dynamic.velocity, gravity[p->dynamic.isFalling].force);
    }
    else
    {
        // Check jump key
        if (IsKeyPressed(KEY_SPACE)) 
        {
            p->dynamic.isGrounded = false;
            p->dynamic.isJumping = true;
            
            // Apply jump force
            p->dynamic.velocity = Vector2Product(p->dynamic.direction, p->dynamic.speed);
            
            // Init rotation easing
            p->rotationEasing.isFinished = false;
        }
    }
    
    // Set player previous position
    p->dynamic.prevPosition = p->transform.position;
    // Update player transform
    p->transform.position = Vector2Add(p->transform.position, p->dynamic.velocity);
    
    // Update rotation easing
    if (!p->rotationEasing.isFinished)
    {
        if (p->rotationEasing.t >= p->rotationEasing.d[p->dynamic.isFalling])
        {
            // Finish easing
            p->rotationEasing.isFinished = true;
            p->rotationEasing.t = 0;
            p->rotationEasing.b += p->rotationEasing.c[p->dynamic.isFalling];
            if (p->rotationEasing.b >= 360) p->rotationEasing.b -= 360;
        }
        else
        {
            p->rotationEasing.t++;
        }
        p->transform.rotation = CubicEaseOut(p->rotationEasing.t, p->rotationEasing.b, p->rotationEasing.c[p->dynamic.isFalling], p->rotationEasing.d[p->dynamic.isFalling]);
    }
    
    // Update player collider
    UpdateSATBox(&p->collider.box, p->transform.position, Vector2FloatProduct(p->collider.box.size, p->transform.scale), p->transform.rotation);
    
    // Set player isGrounded to false since it has to be checked every frame
    p->dynamic.isGrounded = false; 
}

void SetPlayerAsGrounded(Player *p, int landPositionY)
{
    p->dynamic.isGrounded = true;
    p->dynamic.velocity.y = 0;
    p->transform.position.y = landPositionY - p->collider.box.size.y/2;
    
    
    // Finish easing
    if (!p->rotationEasing.isFinished)
    {
        p->rotationEasing.isFinished = true;
        p->rotationEasing.t = 0;
        p->rotationEasing.b += p->rotationEasing.c[p->dynamic.isFalling];
        if (p->rotationEasing.b >= 360) p->rotationEasing.b -= 360;
        p->transform.rotation = CubicEaseOut(p->rotationEasing.t, p->rotationEasing.b, p->rotationEasing.c[p->dynamic.isFalling], p->rotationEasing.d[p->dynamic.isFalling]);
    }
    
    p->dynamic.isJumping = false;
    p->dynamic.isFalling = false;
}

void DrawPlayer (Player p)
{
    DrawTexturePro(p.texture, (Rectangle){0, 0, p.texture.width, p.texture.height}, (Rectangle){p.transform.position.x, 
    p.transform.position.y, p.texture.width, p.texture.height}, (Vector2){p.texture.width/2, 
    p.texture.height/2}, -p.transform.rotation, p.color);
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
                
                // Update collider position
                UpdateAASATTriPosition(&tris[i].collider.tri, tris[i].transform.position);
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

void UpdatePlatfs (BoxGameObject *platfs, Vector2 *sourcePosition, Vector2 playerPosition, Camera2D camera)
{
    for (int i=0; i<3; i++)
    {
        if (platfs[i].state.isActive)
        {
            UpdateOnCameraGameObject(&platfs[i].transform.position, &platfs[i].state, sourcePosition[i], camera);
            
            if (platfs[i].state.isInScreen && platfs[i].transform.position.x-CELL_SIZE*1.5f < playerPosition.x) // CELL_SIZE*1.5 is a safe value for the player-collider distance checking
            {
                // Tri is on the player "colision zone"
                platfs[i].collider.isActive = true;
                
                // Update collider position
                UpdateAASATBoxPosition(&platfs[i].collider.box, platfs[i].transform.position);
            }
            else if (platfs[i].state.isInScreen && platfs[i].transform.position.x + CELL_SIZE*1.5f < player.transform.position.x) // CELL_SIZE*1.5 is a safe value for the player-collider distance checking
            {
                // Platf is out the player "colision zone"
                platfs[i].collider.isActive = false;
            }
        }
    }
}

void CheckPlayerPlatfsCollision (Player *p, BoxGameObject *platfs)
{
    for (int i=0; i<3; i++)
    {
        if (platfs[i].collider.isActive)
        {
            if (SATPolyPolyNCollide(p->collider.box.points, 4, platfs[i].collider.box.points, triNormals, 4))
            {
                // Player collided with a platform
                
                if (p->dynamic.prevPosition.y + CELL_SIZE/2 <= platfs[i].transform.position.y - CELL_SIZE/2)
                {
                     printf("landedPlatf\n");
                    // Player landed on a platform
                    SetPlayerAsGrounded(p, platfs[i].transform.position.y - CELL_SIZE/2);
                    // Set player previous position
                    p->dynamic.prevPosition = p->transform.position;
                }
                else
                {
                    // Set player as dead
                    p->isAlive = false;   
                }
            }
        }
    }
}

void ResetGameplay ()
{
    if (deadFadeIn)
    {
        if (deadFadeAlpha >= 1)
        {
            deadFadeIn = false;
            
            // Reset variables 
            camera.position = Vector2Zero();
            isGameplayStopped = true;
            deadCounter = 0;
            
            player.transform.position = GetOnGridPosition((Vector2){5, gridLenght.y-2});
            player.transform.rotation = 0;
            player.transform.scale = 1;
            
            player.dynamic.prevPosition = player.transform.position;
            player.dynamic.velocity = Vector2Zero();
            player.dynamic.isGrounded = true;
            player.dynamic.isFalling = false;
            player.dynamic.isJumping = false;
            
            player.rotationEasing.t = 0;
            player.rotationEasing.b = 0;
            player.rotationEasing.isFinished = true;
            
            
            UpdateSATBox(&player.collider.box, player.transform.position, Vector2FloatProduct(player.collider.box.size, player.transform.scale), player.transform.rotation);
            
            player.isAlive = true;
            
            for (int i=0; i<3; i++)
            {
                tris[i].state.isActive = true;
                tris[i].collider.isActive = false;
            }
            UpdateTris(tris, trisSourcePosition, player.transform.position, camera);
            
            for (int i=0; i<3; i++)
            {
                platfs[i].state.isActive = true;
                platfs[i].collider.isActive = false;
            }
            UpdatePlatfs (platfs, platfsSourcePosition, player.transform.position, camera); 
        }
        else deadFadeAlpha += 0.05f;
    }
    else
    {
        if (deadFadeAlpha <= 0)
        {
            deadFadeAlpha = 0;
            isDeadFadeFinished = true;
            deadFadeIn = true;
        }
        else
        {
            deadFadeAlpha -= 0.075f;
        }
    }
}