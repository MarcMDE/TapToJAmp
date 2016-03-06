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
#include <math.h>

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
    ParticleEmitter pEmitter;
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

typedef struct Particle
{
    Transform2D transform;
    Vector2 direction;
    Vector2 movementSpeed;
    float rotationSpeed;
    float scaleSpeed;
    int lifeTime;
    Color color;
    bool isActive;
} Particle;

typedef struct SourceParticle
{
    Vector2 direction[2];
    Vector2 movementSpeed[2];
    float rotationSpeed[2];
    float scaleSpeed[2];
    int lifeTime[2];
    Color color;
} SourceParticle;

typedef struct ParticleEmitter
{
    Vector2 position;
    Vector2 offset;
    Vector2 spawnRadius;
    GravityForce gravity;
    int pps; // ParticlesPerSecond
    int framesCounter;
    int particlesCounter;
    SourceParticle source;
    Particle *particles; // Remember to freeb
    bool isActive;
} ParticleEmitter;
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------

// Gameplay screen global variables
static int framesCounter;
static int finishScreen;

static Vector2 gridLenght;
static int groundY;
static GravityForce gravity[2];

static bool isGameplayStopped;

static Camera2D gameElementsCamera;
static Camera2D mainCamera;
static Vector2 onCameraAuxPosition;

static Player player;

// Map variables
static int maxTris;
static int maxPlatfs;

static TriGameObject *tris;
static Vector2 *trisSourcePosition;
static Vector2 triNormals[3];
static Texture2D trisTexture;

static BoxGameObject *platfs;
static Vector2 *platfsSourcePosition;
static Vector2 platfNormals[2];
static Texture2D platfsTexture;

static int attemptsCounter;
static bool isAttemptsCounterActive;
static Vector2 attemptsCounterPosition;
static Vector2 attemptsCounterSourcePosition;

static bool deadCounter;
static bool deadSpan;
static float deadFadeAlpha;
static bool deadFadeIn;
static bool isDeadFadeFinished;

static bool isGamePaused;
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
Vector2 GetOnGridPosition(Vector2 coordinates);
Vector2 GetOnInverseGridPosition(Vector2 coordinates);
void InitPlayer(Player *p, Vector2 coordinates, Vector2 speed, float rotationSpan);
void UpdatePlayer(Player *p);
void SetPlayerAsGrounded(Player *p, int landPositionY);
void DrawPlayer (Player p);
void SetOnCameraPosition (Vector2 *position, Vector2 sourcePosition, Camera2D camera);
Vector2 GetOnCameraPosition (Vector2 position, Camera2D camera);
void UpdateOnCameraGameObject (Vector2 *position, ObjectStates *state, Vector2 sourcePosition, Camera2D camera);
void UpdateTris (TriGameObject *tris, Vector2 *sourcePosition, Vector2 playerPosition, Camera2D camera);
void CheckPlayerTrisCollision (Player *p, TriGameObject *tris);
void UpdatePlatfs (BoxGameObject *platfs, Vector2 *sourcePosition, Vector2 playerPosition, Camera2D camera);
void CheckPlayerPlatfsCollision (Player *p, BoxGameObject *platfs);
void ResetGameplay ();
void InitTri(int index, Vector2 coordinates, int yGridLenght);
void InitPlatf(int index, Vector2 coordinates, int yGridLenght);
void LoadMap();
bool SameColor(Color a, Color b);
void UpdateCustomAASATTriPosition (SATTri *tri, Vector2 position);
void UpdateCustomAASATBoxPosition (SATBox *box, Vector2 position);
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    
    LoadMap();
    
    // Set ground position in the bottom of the desired cell. 
    groundY = GetOnInverseGridPosition((Vector2){0, 2}).y + CELL_SIZE/2;
    
    // Set gravity
    gravity[0].direction = (Vector2){0, 1};
    gravity[0].value = 0.92f;
    gravity[0].force = Vector2FloatProduct(gravity[0].direction, gravity[0].value);
    
    gravity[1].direction = (Vector2){0, 1};
    gravity[1].value = 2.5f;
    gravity[1].force = Vector2FloatProduct(gravity[1].direction, gravity[1].value);
    
    // Set camera
    gameElementsCamera.position = Vector2Zero();
    gameElementsCamera.direction = Vector2Right();
    gameElementsCamera.speed = (Vector2){5.5f, 0};
    gameElementsCamera.isMoving = false;
    
    mainCamera.position = Vector2Zero();
    mainCamera.direction = Vector2Up();
    mainCamera.speed = (Vector2){0, 1};
    mainCamera.isMoving = false;
    
    onCameraAuxPosition = Vector2Zero();
    
    isGameplayStopped = true;
    
    // Counter on player dead (before level reset)
    deadCounter = 0;
    deadSpan = 0.5f * GAME_SPEED;
    deadFadeAlpha = 0;
    deadFadeIn = true;
    isDeadFadeFinished = true;
    
    attemptsCounter = 1;
    isAttemptsCounterActive = true;
    attemptsCounterSourcePosition = GetOnInverseGridPosition((Vector2){9, 10});
    attemptsCounterPosition = attemptsCounterSourcePosition;
    
    isGamePaused = false;
    
    InitPlayer(&player, (Vector2){5, 2}, (Vector2){0, 14}, 0.6f * GAME_SPEED);
    
    // Init Triangles
    trisTexture = LoadTexture("assets/gameplay/tri_main.png");
    UpdateTris(tris, trisSourcePosition, player.transform.position, gameElementsCamera); // Set them as visible if on screen

    UpdateCustomAASATTriPosition(&tris[0].collider.tri, tris[0].transform.position);
    SetNormals(tris[0].collider.tri.points, triNormals, 3, true);
    
    // Init platfsorms
    platfsTexture = LoadTexture("assets/gameplay/platf_main.png");
    UpdatePlatfs (platfs, platfsSourcePosition, player.transform.position, gameElementsCamera); // Set them as visible if on screen

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
                    // Update camera
                    gameElementsCamera.position = Vector2Add(gameElementsCamera.position, Vector2Product(gameElementsCamera.direction, gameElementsCamera.speed));
                    
                    if (player.transform.position.y - mainCamera.position.y < (float)GetScreenHeight()/1.5f)
                    {
                        mainCamera.speed.y = 2;
  
                        if (player.transform.position.y - mainCamera.position.y < (float)GetScreenHeight()/1.75f) mainCamera.position.y = FloatLerp(mainCamera.position.y, player.transform.position.y - (float)GetScreenHeight()/1.75f, mainCamera.speed.y);

                    }
                    else if (mainCamera.position.y < 0)
                    {
                        if (!player.dynamic.isJumping) mainCamera.speed.y += 1;
                        mainCamera.position.y = FloatLerp(mainCamera.position.y, 0, mainCamera.speed.y);
                    }
                    else if (mainCamera.position.y > 0) 
                    {
                        mainCamera.position.y = 0;
                        mainCamera.speed.y = 2;
                    }
                        

                    // Update game objects position before checking the collisions, so the player will see the collision drawed (otherwise it could be skiped)
                    UpdateTris(tris, trisSourcePosition, player.transform.position, gameElementsCamera);
                    UpdatePlatfs (platfs, platfsSourcePosition, player.transform.position, gameElementsCamera); 
                    UpdatePlayer(&player);
                    
                    // Check if player landed on the ground
                    if (player.transform.position.y + player.collider.box.size.y/2 >= groundY) SetPlayerAsGrounded(&player, groundY);
                    // Check if player collided with a triangle
                    CheckPlayerTrisCollision(&player, tris);
                    // Check if player landed (or collided) on a platfsorm.
                    CheckPlayerPlatfsCollision(&player, platfs);
                    
                    if (gameElementsCamera.position.x/CELL_SIZE > gridLenght.x + 10) finishScreen = 1;
                    
                    if (isAttemptsCounterActive)
                    {
                        attemptsCounterPosition = Vector2Sub (attemptsCounterSourcePosition, gameElementsCamera.position);
                        if (attemptsCounterPosition.x < -500)
                        {
                            attemptsCounterPosition = attemptsCounterSourcePosition;
                            isAttemptsCounterActive = false;
                        }
                    }
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
                    gameElementsCamera.isMoving = true;
                    mainCamera.isMoving = true;
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
    DrawRectangle(0, GetOnCameraPosition((Vector2){0, groundY}, mainCamera).y, GetScreenWidth(), 2, BLACK);
    
    // Draw Game Grid
    for (int i=0; i<gridLenght.x+1; i++) DrawRectangle(i*CELL_SIZE, 0, 1, GetScreenHeight(), LIGHTGRAY); // Columns
    for (int i=0; i<gridLenght.y; i++) DrawRectangle(0, i*CELL_SIZE, GetScreenWidth(), 1, LIGHTGRAY); // Rows
    
    // Draw Tris
    for (int i=0; i<maxTris; i++)
    {
        if(tris[i].state.isInScreen) 
        {
            onCameraAuxPosition = GetOnCameraPosition(tris[i].transform.position, mainCamera);
            
            DrawTexturePro(trisTexture, (Rectangle){0, 0, CELL_SIZE/2, CELL_SIZE/2}, (Rectangle){onCameraAuxPosition.x, 
            onCameraAuxPosition.y, CELL_SIZE, CELL_SIZE}, (Vector2){CELL_SIZE/2, CELL_SIZE/2}, 
            tris[i].transform.rotation, WHITE);
        }
    }
    
    for (int i=0; i<maxPlatfs; i++)
    {
        if(platfs[i].state.isInScreen) 
        {
            onCameraAuxPosition = GetOnCameraPosition(platfs[i].transform.position, mainCamera);
            
            DrawTexturePro(platfsTexture, (Rectangle){0, 0, CELL_SIZE, CELL_SIZE}, (Rectangle){onCameraAuxPosition.x, 
            onCameraAuxPosition.y, CELL_SIZE, CELL_SIZE}, (Vector2){CELL_SIZE/2, CELL_SIZE/2}, 
            platfs[i].transform.rotation, WHITE);
        }
        
        /*
        for (int j=0; j<4; j++)
        {
            onCameraAuxPosition = GetOnCameraPosition(platfs[i].collider.box.points[j], mainCamera);
            if (platfs[i].collider.isActive) DrawCircleV(onCameraAuxPosition, 5, GREEN);
        }
        */
    }
    
    if (isAttemptsCounterActive) DrawText(FormatText("%i", attemptsCounter), attemptsCounterPosition.x, attemptsCounterPosition.y, 150, DARKGRAY);
    
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
    free(tris);
    free(platfs);
    free(trisSourcePosition);
    free(platfsSourcePosition);
    free(player.pEmitter.particles)
    
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
    position.y = coordinates.y - CELL_SIZE/2;
    return position; 
}

Vector2 GetOnInverseGridPosition(Vector2 coordinates)
{
    Vector2 position;
    Vector2Scale(&coordinates, CELL_SIZE);
    position.x = coordinates.x + CELL_SIZE/2;
    position.y = GetScreenHeight() - coordinates.y - CELL_SIZE/2;
    return position; 
}

void InitPlayer(Player *p, Vector2 coordinates, Vector2 speed, float rotationSpan)
{
    // Init p transform
    p->transform.position = GetOnInverseGridPosition(coordinates);
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
    
    // Init particles emitter
    p->pEmitter.offset = (Vector2){-CELL_SIZE/2 + 10, CELL_SIZE/2-10};
    p->pEmitter.position = Vector2Add(p->transform.position, p->pEmitter.offset);
    p->pEmitter.spawnRadius = 0;
    p->pEmitter.gravity.direction = (Vector2){1, 1};
    p->pEmitter.gravity.value = (Vector2){0.5f, 0.75f};
    p->pEmitter.gravity.force = Vector2Product(p->pEmitter.gravity.direction,  p->pEmitter.gravity.value);
    p->pEmitter.pps = 70;
    p->pEmitter.framesCounter = 0;
    p->pEmitter.particlesCounter = 0;
    
    // particle emitter source particle
    p->pEmitter.source.direction[0] = (Vector2){-1, 0};
    p->pEmitter.source.direction[1] = (Vector2){-1, -0.2f};
    p->pEmitter.source.movementSpeed[0] = (Vector2){2, 0};
    p->pEmitter.source.movementSpeed[1] = (Vector2){2, 2};
    p->pEmitter.source.rotationSpeed[0] = 0.5f;
    p->pEmitter.source.rotationSpeed[1] = 2;
    p->pEmitter.particles = malloc(sizeof(Particle)*200); // Remember to free
    
    
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
        if (IsKeyDown(KEY_SPACE)) 
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
    onCameraAuxPosition = GetOnCameraPosition(p.transform.position, mainCamera);
    
    DrawTexturePro(p.texture, (Rectangle){0, 0, CELL_SIZE, CELL_SIZE}, (Rectangle){onCameraAuxPosition.x, 
    onCameraAuxPosition.y, CELL_SIZE, CELL_SIZE}, (Vector2){CELL_SIZE/2, 
    CELL_SIZE/2}, -p.transform.rotation, p.color);
}

void SetOnCameraPosition (Vector2 *position, Vector2 sourcePosition, Camera2D camera)
{
    *position = Vector2Sub(sourcePosition, camera.position);
}

Vector2 GetOnCameraPosition (Vector2 position, Camera2D camera)
{
    return Vector2Sub(position, camera.position);
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
    for (int i=0; i<maxTris; i++)
    {
        if (tris[i].state.isActive)
        {
            UpdateOnCameraGameObject(&tris[i].transform.position, &tris[i].state, sourcePosition[i], camera);
            
            if (tris[i].state.isInScreen && tris[i].transform.position.x-CELL_SIZE*1.5f < playerPosition.x) // CELL_SIZE*1.5 is a safe value for the player-collider distance checking
            {
                // Tri is on the player "colision zone"
                tris[i].collider.isActive = true;
                
                // Update collider position
                UpdateCustomAASATTriPosition(&tris[i].collider.tri, tris[i].transform.position);
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
    for (int i=0; i<maxTris; i++)
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
    for (int i=0; i<maxPlatfs; i++)
    {
        if (platfs[i].state.isActive)
        {
            UpdateOnCameraGameObject(&platfs[i].transform.position, &platfs[i].state, sourcePosition[i], camera);
            
            if (platfs[i].state.isInScreen && platfs[i].transform.position.x - CELL_SIZE * 1.5f < playerPosition.x && 
            platfs[i].transform.position.x + CELL_SIZE * 1.5f > player.transform.position.x) // CELL_SIZE*1.5 is a safe value for the player-collider distance checking
            {
                // Platf is on the player "colision zone"
                platfs[i].collider.isActive = true;
                
                // Update collider position
                UpdateCustomAASATBoxPosition(&platfs[i].collider.box, platfs[i].transform.position);
            }
            else platfs[i].collider.isActive = false;
        }
    }
}

void CheckPlayerPlatfsCollision (Player *p, BoxGameObject *platfs)
{
    for (int i=0; i<maxPlatfs; i++)
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
            gameElementsCamera.position = Vector2Zero();
            isGameplayStopped = true;
            deadCounter = 0;
            
            mainCamera.position = Vector2Zero();
            
            player.transform.position = GetOnInverseGridPosition((Vector2){5, 2});
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
            
            isAttemptsCounterActive = true;
            attemptsCounterPosition = attemptsCounterSourcePosition;
            attemptsCounter++;
            
            UpdateSATBox(&player.collider.box, player.transform.position, Vector2FloatProduct(player.collider.box.size, player.transform.scale), player.transform.rotation);
            
            player.isAlive = true;
            
            for (int i=0; i<maxTris; i++)
            {
                tris[i].state.isActive = true;
                tris[i].collider.isActive = false;
            }
            UpdateTris(tris, trisSourcePosition, player.transform.position, gameElementsCamera);
            
            for (int i=0; i<maxPlatfs; i++)
            {
                platfs[i].state.isActive = true;
                platfs[i].collider.isActive = false;
            }
            UpdatePlatfs (platfs, platfsSourcePosition, player.transform.position, gameElementsCamera); 
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

void InitTri(int index, Vector2 coordinates, int yGridLenght)
{
        tris[index].transform.position = GetOnGridPosition(coordinates);
        tris[index].transform.position.y -= yGridLenght * CELL_SIZE - GetScreenHeight();
        tris[index].transform.scale = 1;
        tris[index].transform.rotation = 0;
        trisSourcePosition[index] = tris[index].transform.position;
        
        InitSATTri(&tris[index].collider.tri, tris[index].transform.position, (Vector2){CELL_SIZE, CELL_SIZE}, tris[index].transform.rotation);
        tris[index].collider.isActive = false;
    
        tris[index].state.isActive = true;
        tris[index].state.isInScreen = false;
        tris[index].state.isUp = true;
}

void InitPlatf(int index, Vector2 coordinates, int yGridLenght)
{
    platfs[index].transform.position = GetOnGridPosition(coordinates);
    platfs[index].transform.position.y -= yGridLenght * CELL_SIZE - GetScreenHeight();
    platfs[index].transform.scale = 1;
    platfs[index].transform.rotation = 0;
    platfsSourcePosition[index] = platfs[index].transform.position;

    InitSATBox(&platfs[index].collider.box, platfs[index].transform.position, (Vector2){CELL_SIZE, CELL_SIZE}, platfs[index].transform.rotation);
    platfs[index].collider.isActive = false;

    platfs[index].state.isActive = true;
    platfs[index].state.isInScreen = false;
    platfs[index].state.isUp = true;
}

void LoadMap ()
{
    Image mapImage;
    Color *mapImagePixels;
    
    int trisCounter = 0;
    int platfsCounter = 0;
    
    maxTris = 0;
    maxPlatfs = 0;
    
    mapImage = LoadImage("maps/map_01.bmp");
    gridLenght.x = mapImage.width;
    gridLenght.y = mapImage.height;
    mapImagePixels = malloc(sizeof(Color)*(gridLenght.x*gridLenght.y));
    
    mapImagePixels = GetImageData(mapImage);
    
    for (int i=0; i<gridLenght.x*gridLenght.y; i++)
    {
        if (SameColor(mapImagePixels[i], (Color){255, 0, 0, 255})) maxTris++;
        else if (SameColor(mapImagePixels[i], (Color){0, 255, 0, 255})) maxPlatfs++;
    }
    
    tris = malloc(sizeof(TriGameObject) * maxTris);
    trisSourcePosition = malloc(sizeof(TriGameObject) * maxTris);
    platfs = malloc(sizeof(BoxGameObject) * maxPlatfs);
    platfsSourcePosition = malloc(sizeof(BoxGameObject) * maxPlatfs);
    
    for (int y=0; y<gridLenght.y; y++)
    {
        for (int x=0; x<gridLenght.x; x++)
        {
            if (SameColor(mapImagePixels[y*(int)gridLenght.x+x], (Color){255, 0, 0, 255}))
            {
                // Init tri
                InitTri(trisCounter, (Vector2){x, y}, gridLenght.y-1);
                trisCounter++;
            }
            else if (SameColor(mapImagePixels[y*(int)gridLenght.x+x], (Color){0, 255, 0, 255}))
            {
                // Init platf
                InitPlatf(platfsCounter, (Vector2){x, y}, gridLenght.y-1);
                platfsCounter++;
            }
        }
    }
    
    printf("Tris: %i\n", maxTris);
    
    free(mapImagePixels);
    UnloadImage(mapImage);
}

bool SameColor(Color a, Color b)
{
    if (a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a)
    {
        return true;
    }
    return false;
}

void UpdateCustomAASATTriPosition (SATTri *tri, Vector2 position)
{
    if (tri->position.x != position.x || tri->position.y != position.y)
    {
        Vector2 size;
        
        tri->position = position;
        size = tri->size;
        
        // Set half-size for faster use on points asignation
        Vector2Scale(&size, 0.5f);
        
        // Set tri corners position
        tri->points[0] = Vector2Add(tri->position, Vector2Product((Vector2){-1, 1}, size));
        tri->points[1] = Vector2Add(tri->position, Vector2Product((Vector2){0, -1}, size));
        tri->points[2] = Vector2Add(tri->position, Vector2Product((Vector2){1, 1}, size));
        
        // Set top point 1 pixel down
        tri->points[1].y+= 1;
    }
}

void UpdateCustomAASATBoxPosition (SATBox *box, Vector2 position)
{
    if (box->position.x != position.x || box->position.y != position.y)
    {
        Vector2 size;
        
        box->position = position;
        size = box->size;
        
        Vector2Scale(&size, 0.5f);
        
        box->points[0] = Vector2Sub(box->position, Vector2Product(Vector2One(), size));
        box->points[1] = Vector2Add(box->position, Vector2Product((Vector2){1, -1}, size));
        box->points[2] = Vector2Add(box->position, Vector2Product(Vector2One(), size));
        box->points[3] = Vector2Sub(box->position, Vector2Product((Vector2){1, -1}, size));
        
        // Set box bot points 1 pexel up 
        box->points[2].y-=1;
        box->points[3].y-=1;
    }
}
