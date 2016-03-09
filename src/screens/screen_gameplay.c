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
#include <time.h> // RAND_MAX

#define GAME_SPEED 60
#define PLAYER_PARTICLES 60
#define PLAYER_ONDEAD_PARTICLES 50
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

typedef struct SimpleEasing
{
    float t, b, c, d;
    bool isFinished;
}SimpleEasing;

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
    Vector2 velocity;
    Vector2 movementSpeed;
    float rotationSpeed;
    float scaleSpeed;
    int lifeTime;
    Color color;
    bool isActive;
} Particle;

typedef struct SourceParticle
{
    float rotation[2];
    float scale[2];
    Vector2 direction[2];
    Vector2 movementSpeed[2];
    float rotationSpeed[2];
    float scaleSpeed[2];
    int lifeTime[2];
    Color color;
    Texture2D texture;
} SourceParticle;

typedef struct ParticleEmitter
{
    Vector2 position;
    Vector2 offset;
    float spawnRadius;
    GravityForce gravity;
    float ppf; // ParticlesPerFrame
    float frameParticles; // Total frame particles
    float remainingParticles; // Decimal particles left
    int particlesAmount; // Particles that will be spawned on the current frame
    SourceParticle source;
    Particle *particles; // Remember to freeb
    bool isBurst;
    bool isActive;
} ParticleEmitter;

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
    SimpleEasing onDeadScaleEasing;
    int onDeadCircleSize;
    ParticleEmitter onDeadPEmitter;
}Player;

typedef struct Bar
{
    Rectangle back;
    Rectangle front;
    bool isActive;
}Bar;
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

static Bar progressBar;

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
void UpdateOnCameraGameObject (Vector2 *position, ObjectStates *state, Vector2 sourcePosition, Camera2D elementsCamera, Camera2D camera);
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
float GetRandomFloat(float min, float max);
Vector2 GetRandomVector2(Vector2 v1, Vector2 v2);
void UpdateParticleEmitter (ParticleEmitter *pE, int maxParticles, Vector2 position);
void InitParticle (Particle *p, SourceParticle s, float spawnRadius, Vector2 pEPosition);
void UpdateParticle(Particle *p, GravityForce gravity);
void KillPlayer (Player *p);
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
    gravity[0].value = 1.15f;
    gravity[0].force = Vector2FloatProduct(gravity[0].direction, gravity[0].value);
    
    gravity[1].direction = (Vector2){0, 1};
    gravity[1].value = 2.5f;
    gravity[1].force = Vector2FloatProduct(gravity[1].direction, gravity[1].value);
    
    // Set camera
    gameElementsCamera.position = Vector2Zero();
    gameElementsCamera.direction = Vector2Right();
    gameElementsCamera.speed = (Vector2){6.65f, 0};
    gameElementsCamera.isMoving = false;
    
    mainCamera.position = Vector2Zero();
    mainCamera.direction = Vector2Up();
    mainCamera.speed = (Vector2){0, 5.5f};
    mainCamera.isMoving = false;
    
    onCameraAuxPosition = Vector2Zero();
    
    isGameplayStopped = true;
    
    // Counter on player dead (before level reset)
    deadCounter = 0;
    deadSpan = 0.75f * GAME_SPEED;
    deadFadeAlpha = 0;
    deadFadeIn = true;
    isDeadFadeFinished = true;
    
    attemptsCounter = 1;
    isAttemptsCounterActive = true;
    attemptsCounterSourcePosition = GetOnInverseGridPosition((Vector2){9, 10});
    attemptsCounterPosition = attemptsCounterSourcePosition;
    
    isGamePaused = false;
    
    InitPlayer(&player, (Vector2){5, 2}, (Vector2){0, 15.75f}, 0.6f * GAME_SPEED);
    
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
    
    progressBar.back = (Rectangle){200, 10, GetScreenWidth() - 400, 12};
    progressBar.front = (Rectangle){200, 10, 0, 12};
    progressBar.isActive = true;
    
    srand(time(NULL)); 
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
                    
                    
                    if (player.transform.position.y - mainCamera.position.y < (float)GetScreenHeight()/1.4f)
                    {
                        mainCamera.speed.y = 4.5f;
                        if (player.transform.position.y - mainCamera.position.y < (float)GetScreenHeight()/1.6f && !player.dynamic.isJumping) 
                        {
                            mainCamera.position.y = FloatLerp(mainCamera.position.y, player.transform.position.y - (float)GetScreenHeight()/1.6f, mainCamera.speed.y);
                        }
                    }
                    else if (mainCamera.position.y < 0)
                    {
                        mainCamera.speed.y += 0.5f;
                        mainCamera.position.y = FloatLerp(mainCamera.position.y, 0, mainCamera.speed.y);
                    }
                    else if (mainCamera.position.y > 0) 
                    {
                        mainCamera.position.y = 0;
                        //mainCamera.speed.y = 4.5f;
                    }
                    
                    if (progressBar.front.width < progressBar.back.width && progressBar.isActive) 
                    {
                        progressBar.front.width = progressBar.back.width *  (gameElementsCamera.position.x / (gridLenght.x * CELL_SIZE));
                    }
                    else if (!progressBar.isActive)
                    {
                        progressBar.front.width = progressBar.back.width;
                        progressBar.isActive = false;
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
                        UpdateParticleEmitter(&player.onDeadPEmitter, PLAYER_ONDEAD_PARTICLES, player.transform.position);
                        
                        player.onDeadScaleEasing.t = deadCounter;
                        player.onDeadCircleSize = CubicEaseOut(player.onDeadScaleEasing.t, player.onDeadScaleEasing.b, player.onDeadScaleEasing.c, player.onDeadScaleEasing.d);
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
            
            DrawTexturePro(trisTexture, (Rectangle){0, 0, CELL_SIZE, CELL_SIZE}, (Rectangle){onCameraAuxPosition.x, 
            onCameraAuxPosition.y, CELL_SIZE, CELL_SIZE}, (Vector2){CELL_SIZE/2, CELL_SIZE/2}, 
            tris[i].transform.rotation, WHITE);
            
            // Debug collision points
            for (int j=0; j<3; j++)
            {
                onCameraAuxPosition = GetOnCameraPosition(tris[i].collider.tri.points[j], mainCamera);
                if (tris[i].collider.isActive) DrawCircleV(onCameraAuxPosition, 5, GREEN);
            }
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
            
            // Debug collision points
            for (int j=0; j<4; j++)
            {
                onCameraAuxPosition = GetOnCameraPosition(platfs[i].collider.box.points[j], mainCamera);
                if (platfs[i].collider.isActive) DrawCircleV(onCameraAuxPosition, 5, GREEN);
            }
        }
    }
    
    if (isAttemptsCounterActive) DrawText(FormatText("%i", attemptsCounter), attemptsCounterPosition.x, attemptsCounterPosition.y, 150, DARKGRAY);
    
    DrawPlayer(player);
    
    DrawRectangleRec(progressBar.back, LIGHTGRAY);
    DrawRectangleRec(progressBar.front, RED);
    
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
    free(player.pEmitter.particles);
    free(player.onDeadPEmitter.particles);
    
    UnloadTexture(player.texture);
    UnloadTexture(trisTexture);
    UnloadTexture(platfsTexture);
    UnloadTexture(player.pEmitter.source.texture);
    UnloadTexture(player.onDeadPEmitter.source.texture);
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
    p->pEmitter.offset = (Vector2){-CELL_SIZE/2 + 5, CELL_SIZE/2-3};
    p->pEmitter.position = Vector2Add(p->transform.position, p->pEmitter.offset);
    p->pEmitter.spawnRadius = 0;
    p->pEmitter.gravity.direction = (Vector2){0.65f, 1};
    p->pEmitter.gravity.value = 0.03f;
    p->pEmitter.gravity.force = Vector2FloatProduct(p->pEmitter.gravity.direction,  p->pEmitter.gravity.value);
    p->pEmitter.ppf = 0.7f;
    p->pEmitter.frameParticles = 0;
    p->pEmitter.particlesAmount = 0;
    p->pEmitter.remainingParticles = 0;
    p->pEmitter.isBurst = false;
    
    // particle emitter source particle
    p->pEmitter.source.direction[0] = (Vector2){-1, -1};
    p->pEmitter.source.direction[1] = (Vector2){-1, -1};
    p->pEmitter.source.rotation[0] = 0;
    p->pEmitter.source.rotation[1] = 360;
    p->pEmitter.source.scale[0] = 2.5f;
    p->pEmitter.source.scale[1] = 3.75f;
    p->pEmitter.source.movementSpeed[0] = (Vector2){2.5f, 0.6f};
    p->pEmitter.source.movementSpeed[1] = (Vector2){4, 0.7f};
    p->pEmitter.source.rotationSpeed[0] = 0.25f;
    p->pEmitter.source.rotationSpeed[1] = 1;
    p->pEmitter.source.scaleSpeed[0] = -0.0085f;
    p->pEmitter.source.scaleSpeed[1] = -0.012f;
    p->pEmitter.source.lifeTime[0] = 0.6f * GAME_SPEED;
    p->pEmitter.source.lifeTime[1] = 0.85f * GAME_SPEED;
    p->pEmitter.source.color = (Color){0, 255, 50, 175};
    p->pEmitter.source.texture = LoadTexture("assets/gameplay/particle_main.png");
    
    p->pEmitter.particles = malloc(sizeof(Particle)*PLAYER_PARTICLES); // Remember to free
    p->pEmitter.isActive = true;
    
    for (int i=0; i<PLAYER_PARTICLES; i++)
    {
        p->pEmitter.particles[i].isActive = false;
    }

    p->onDeadPEmitter.offset = Vector2Zero();
    p->onDeadPEmitter.position = p->transform.position;
    p->onDeadPEmitter.spawnRadius = 0;
    p->onDeadPEmitter.gravity.direction = Vector2Zero();
    p->onDeadPEmitter.gravity.value = 0;
    p->onDeadPEmitter.gravity.force = Vector2FloatProduct(p->onDeadPEmitter.gravity.direction,  p->onDeadPEmitter.gravity.value);
    p->onDeadPEmitter.ppf = PLAYER_ONDEAD_PARTICLES;
    p->onDeadPEmitter.frameParticles = 0;
    p->onDeadPEmitter.particlesAmount = 0;
    p->onDeadPEmitter.remainingParticles = 0;
    p->onDeadPEmitter.isBurst = true;
    
    p->onDeadPEmitter.source.direction[0] = (Vector2){-1, -1};
    p->onDeadPEmitter.source.direction[1] = (Vector2){1, 1};
    p->onDeadPEmitter.source.rotation[0] = 0;
    p->onDeadPEmitter.source.rotation[1] = 360;
    p->onDeadPEmitter.source.scale[0] = 0.5f;
    p->onDeadPEmitter.source.scale[1] = 1;
    p->onDeadPEmitter.source.movementSpeed[0] = (Vector2){5.5f, 5.5f};
    p->onDeadPEmitter.source.movementSpeed[1] = (Vector2){6.5, 6.5};
    p->onDeadPEmitter.source.rotationSpeed[0] = 3.5f;
    p->onDeadPEmitter.source.rotationSpeed[1] = 6;
    p->onDeadPEmitter.source.scaleSpeed[0] = 0.05f;
    p->onDeadPEmitter.source.scaleSpeed[1] = 0.1f;
    p->onDeadPEmitter.source.lifeTime[0] = deadSpan;
    p->onDeadPEmitter.source.lifeTime[1] = deadSpan;
    p->onDeadPEmitter.source.color = (Color){0, 255, 50, 255};
    p->onDeadPEmitter.source.texture = LoadTexture("assets/gameplay/particle_dead.png");
    
    p->onDeadPEmitter.particles = malloc(sizeof(Particle)*PLAYER_ONDEAD_PARTICLES); // Remember to free
    p->onDeadPEmitter.isActive = false;
    
    for (int i=0; i<PLAYER_ONDEAD_PARTICLES; i++)
    {
        p->onDeadPEmitter.particles[i].isActive = false;
    }
    
    p->onDeadScaleEasing.isFinished = true;
    p->onDeadScaleEasing.t = 0;
    p->onDeadScaleEasing.b = 1;
    p->onDeadScaleEasing.c = 200;
    p->onDeadScaleEasing.d = deadSpan;
    
    p->onDeadCircleSize = p->onDeadScaleEasing.b;
    
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
    
    UpdateParticleEmitter(&p->pEmitter, PLAYER_PARTICLES, p->transform.position);
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
    if (p.isAlive)
    {
        for (int i=0; i<PLAYER_PARTICLES; i++)
        {
            if (p.pEmitter.particles[i].isActive)
            {
                onCameraAuxPosition = GetOnCameraPosition(p.pEmitter.particles[i].transform.position, mainCamera);
                
                DrawTexturePro(p.pEmitter.source.texture, (Rectangle){0, 0, p.pEmitter.source.texture.width, p.pEmitter.source.texture.height}, 
                (Rectangle){onCameraAuxPosition.x, onCameraAuxPosition.y, p.pEmitter.source.texture.width * p.pEmitter.particles[i].transform.scale, 
                p.pEmitter.source.texture.height * p.pEmitter.particles[i].transform.scale}, (Vector2){p.pEmitter.source.texture.width/2, p.pEmitter.source.texture.height/2}, 
                -p.pEmitter.particles[i].transform.rotation, p.pEmitter.particles[i].color);
            }
        }
        
        onCameraAuxPosition = GetOnCameraPosition(p.transform.position, mainCamera);
        
        DrawTexturePro(p.texture, (Rectangle){0, 0, CELL_SIZE, CELL_SIZE}, (Rectangle){onCameraAuxPosition.x, 
        onCameraAuxPosition.y, CELL_SIZE, CELL_SIZE}, (Vector2){CELL_SIZE/2, 
        CELL_SIZE/2}, -p.transform.rotation, p.color);
    }
    else
    {
        onCameraAuxPosition = GetOnCameraPosition(p.transform.position, mainCamera);
        //DrawCircleV(onCameraAuxPosition, p.onDeadCircleSize, Fade(BLUE, 0.4f));
        
        for (int i=0; i<PLAYER_ONDEAD_PARTICLES; i++)
        {
            if (p.onDeadPEmitter.particles[i].isActive)
            {
                onCameraAuxPosition = GetOnCameraPosition(p.onDeadPEmitter.particles[i].transform.position, mainCamera);
                
                DrawTexturePro(p.onDeadPEmitter.source.texture, (Rectangle){0, 0, p.onDeadPEmitter.source.texture.width, p.onDeadPEmitter.source.texture.height}, 
                (Rectangle){onCameraAuxPosition.x, onCameraAuxPosition.y, p.onDeadPEmitter.source.texture.width * p.onDeadPEmitter.particles[i].transform.scale, 
                p.onDeadPEmitter.source.texture.height * p.onDeadPEmitter.particles[i].transform.scale}, (Vector2){p.onDeadPEmitter.source.texture.width * p.onDeadPEmitter.particles[i].transform.scale/2, 
                p.onDeadPEmitter.source.texture.height * p.onDeadPEmitter.particles[i].transform.scale/2}, 
                -p.onDeadPEmitter.particles[i].transform.rotation, p.onDeadPEmitter.particles[i].color);
            }
        }
    }
}

void SetOnCameraPosition (Vector2 *position, Vector2 sourcePosition, Camera2D camera)
{
    *position = Vector2Sub(sourcePosition, camera.position);
}

Vector2 GetOnCameraPosition (Vector2 position, Camera2D camera)
{
    return Vector2Sub(position, camera.position);
}

void UpdateOnCameraGameObject (Vector2 *position, ObjectStates *state, Vector2 sourcePosition, Camera2D elementsCamera, Camera2D camera)
{
    SetOnCameraPosition(position, sourcePosition, elementsCamera);
    onCameraAuxPosition = GetOnCameraPosition(*position, camera);

    // Set object un-active if leaves the screen (from left)
    if (position->x + CELL_SIZE/2 < 0) 
    {
        state->isInScreen = false;
        state->isActive = false;
    }
    else if (position->x - CELL_SIZE/2 < GetScreenWidth() && onCameraAuxPosition.y + CELL_SIZE/2 > 0 && onCameraAuxPosition.y - CELL_SIZE/2 < GetScreenHeight())
    {
        state->isInScreen = true;
    }
    else state->isInScreen = false;

}

void UpdateTris (TriGameObject *tris, Vector2 *sourcePosition, Vector2 playerPosition, Camera2D camera)
{
    for (int i=0; i<maxTris; i++)
    {
        if (tris[i].state.isActive)
        {
            UpdateOnCameraGameObject(&tris[i].transform.position, &tris[i].state, sourcePosition[i], camera, mainCamera);
            
            if (tris[i].state.isInScreen && CheckCollisionRecs((Rectangle){playerPosition.x - (CELL_SIZE/2 + 20), playerPosition.y - (CELL_SIZE/2 + 20), CELL_SIZE + 40, CELL_SIZE + 40}, 
            (Rectangle){tris[i].transform.position.x - CELL_SIZE/2, tris[i].transform.position.y - CELL_SIZE/2, CELL_SIZE, CELL_SIZE}))
            {
                // Tri is on the player "colision zone"
                tris[i].collider.isActive = true;
                
                // Update collider position
                UpdateCustomAASATTriPosition(&tris[i].collider.tri, tris[i].transform.position);
            }
            else
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
                KillPlayer(p);
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
            UpdateOnCameraGameObject(&platfs[i].transform.position, &platfs[i].state, sourcePosition[i], camera, mainCamera);
            
            if (platfs[i].state.isInScreen && CheckCollisionRecs((Rectangle){playerPosition.x - (CELL_SIZE/2 + 20), playerPosition.y - (CELL_SIZE/2 + 20), CELL_SIZE + 40, CELL_SIZE + 40}, 
            (Rectangle){platfs[i].transform.position.x - CELL_SIZE/2, platfs[i].transform.position.y - CELL_SIZE/2, CELL_SIZE, CELL_SIZE}))
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
                    // Player landed on a platform
                    SetPlayerAsGrounded(p, platfs[i].transform.position.y - CELL_SIZE/2);
                    // Set player previous position
                    p->dynamic.prevPosition = p->transform.position;
                }
                else
                {
                    // Set player as dead
                    KillPlayer(p);
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
            mainCamera.speed.y = 4.5f;
            mainCamera.position = Vector2Zero();
            
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
            
            for (int i=0; i<PLAYER_PARTICLES; i++)
            {
                player.pEmitter.particles[i].isActive = false;
            }
            
            player.onDeadPEmitter.isActive = false;
            
            for (int i=0; i<PLAYER_ONDEAD_PARTICLES; i++)
            {
                player.onDeadPEmitter.particles[i].isActive = false;
            }
            
            player.onDeadScaleEasing.t = 0;
            player.onDeadCircleSize = 0;
            
            isAttemptsCounterActive = true;
            attemptsCounterPosition = attemptsCounterSourcePosition;
            attemptsCounter++;
            
            UpdateSATBox(&player.collider.box, player.transform.position, Vector2FloatProduct(player.collider.box.size, player.transform.scale), player.transform.rotation);
            
            player.isAlive = true;
            
            for (int i=0; i<maxTris; i++)
            {
                tris[i].state.isActive = true;
                tris[i].state.isInScreen = false;
                tris[i].collider.isActive = false;
            }
            UpdateTris(tris, trisSourcePosition, player.transform.position, gameElementsCamera);
            
            for (int i=0; i<maxPlatfs; i++)
            {
                platfs[i].state.isActive = true;
                platfs[i].state.isInScreen = false;
                platfs[i].collider.isActive = false;
            }
            UpdatePlatfs (platfs, platfsSourcePosition, player.transform.position, gameElementsCamera); 
            
            progressBar.front.width = 0;
            progressBar.isActive = true;
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
    
    mapImage = LoadImage("maps/map_02.bmp");
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

float GetRandomFloat(float min, float max)
{
    return (max-min) * ((float)rand() / (float) RAND_MAX) + min;
}

Vector2 GetRandomVector2(Vector2 v1, Vector2 v2)
{
    return (Vector2){GetRandomFloat(v1.x, v2.x), GetRandomFloat(v1.y, v2.y)};
}

void UpdateParticleEmitter (ParticleEmitter *pE, int maxParticles, Vector2 position)
{
    if (pE->isActive)
    {
        pE->position = Vector2Add(position, pE->offset);
        
        if (!pE->isBurst)
        {
            pE->frameParticles = pE->ppf + pE->remainingParticles;
            pE->particlesAmount = pE->frameParticles/1;
            pE->remainingParticles = fmod(pE->frameParticles, 1);
            
            for (int i=0; i<maxParticles; i++)
            {
                if (!pE->particles[i].isActive)
                {
                    // Init particle
                    if (pE->particlesAmount > 0)
                    {
                    InitParticle(&pE->particles[i], pE->source, pE->spawnRadius, pE->position);
                    pE->particlesAmount--;
                    }
                    else i=maxParticles;
                }
            }
        }
        for (int i=0; i<maxParticles; i++)
        {
            UpdateParticle(&pE->particles[i], pE->gravity);
            //if (pE->particles[i].isActive) printf("P: %i \n", i);
        }
    }
}

void InitParticle (Particle *p, SourceParticle s, float spawnRadius, Vector2 pEPosition)
{   
    if (spawnRadius!= 0)
    {
        p->transform.position = Vector2Add(pEPosition, (Vector2){0, GetRandomFloat(0, -spawnRadius)});
        Vector2Rotate(&p->transform.position, pEPosition, GetRandomValue(0, 360));
    }   
    else p->transform.position = pEPosition;
    
    p->transform.rotation = GetRandomFloat(s.rotation[0], s.rotation[1]);
    p->transform.scale = GetRandomFloat(s.scale[0], s.scale[1]);
    
    p->direction = GetRandomVector2(s.direction[0], s.direction[1]);
    p->movementSpeed = GetRandomVector2(s.movementSpeed[0], s.movementSpeed[1]);
    p->velocity = Vector2Product(p->movementSpeed, p->direction);
    p->rotationSpeed = GetRandomFloat(s.rotationSpeed[0], s.rotationSpeed[1]);
    p->scaleSpeed = GetRandomFloat(s.scaleSpeed[0], s.scaleSpeed[1]);
    p->lifeTime = GetRandomValue(s.lifeTime[0], s.lifeTime[1]);
    
    p->color = s.color;
    
    p->isActive = true;
}

void UpdateParticle(Particle *p, GravityForce gravity)
{
    if (p->isActive)
    {
        if (p->lifeTime > 0)
        {
            p->velocity = Vector2Add(p->velocity, gravity.force);
            p->transform.position = Vector2Add(p->transform.position, p->velocity);
            p->transform.rotation += p->rotationSpeed;
            p->transform.scale += p->scaleSpeed;
            
            p->lifeTime--;
            
            if (p->lifeTime <= 0) p->isActive = false;
        }
        else p->isActive = false;
    }
}

void KillPlayer (Player *p)
{
    p->isAlive = false;   
    
    p->onDeadScaleEasing.isFinished = false;
    p->onDeadPEmitter.isActive = true;
    
    // Init onDeadPEmitter
    p->onDeadPEmitter.position = p->transform.position;
    
    for (int i=0; i<PLAYER_ONDEAD_PARTICLES; i++)
    {
        // Init particle
        InitParticle(&p->onDeadPEmitter.particles[i], p->onDeadPEmitter.source, p->onDeadPEmitter.spawnRadius, p->onDeadPEmitter.position);
    }
    
}