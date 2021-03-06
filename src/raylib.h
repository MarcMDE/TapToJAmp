﻿/**********************************************************************************************
*
*   raylib 1.4.0 (www.raylib.com)
*
*   A simple and easy-to-use library to learn videogames programming
*
*   Features:
*     Library written in plain C code (C99)
*     Uses C# PascalCase/camelCase notation
*     Hardware accelerated with OpenGL (1.1, 3.3 or ES2)
*     Unique OpenGL abstraction layer [rlgl]
*     Powerful fonts module with SpriteFonts support (including AngelCode fonts and TTF)
*     Multiple textures support, including compressed formats and mipmaps generation
*     Basic 3d support for Shapes, Models, Heightmaps and Billboards
*     Powerful math module for Vector and Matrix operations [raymath]
*     Audio loading and playing with streaming support (WAV and OGG)
*     Multiplatform support, including Android devices, Raspberry Pi and HTML5
*
*   Used external libs:
*     GLFW3 (www.glfw.org) for window/context management and input
*     GLAD for OpenGL extensions loading (3.3 Core profile, only PLATFORM_DESKTOP)
*     stb_image (Sean Barret) for images loading (JPEG, PNG, BMP, TGA, PSD, GIF, HDR, PIC)
*     stb_image_write (Sean Barret) for image writting (PNG)
*     stb_vorbis (Sean Barret) for ogg audio loading
*     stb_truetype (Sean Barret) for ttf fonts loading
*     OpenAL Soft for audio device/context management
*     tinfl for data decompression (DEFLATE algorithm)
*
*   Some design decisions:
*     32bit Colors - All defined color are always RGBA (struct Color is 4 byte)
*     One custom default font is loaded automatically when InitWindow()
*     If using OpenGL 3.3 or ES2, several vertex buffers (VAO/VBO) are created to manage lines-triangles-quads
*     If using OpenGL 3.3 or ES2, two default shaders are loaded automatically (internally defined)
*
*   -- LICENSE --
*
*   raylib is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   Copyright (c) 2013 Ramon Santamaria (@raysan5)
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

#ifndef RAYLIB_H
#define RAYLIB_H

// Choose your platform here or just define it at compile time: -DPLATFORM_DESKTOP
//#define PLATFORM_DESKTOP      // Windows, Linux or OSX
//#define PLATFORM_ANDROID      // Android device
//#define PLATFORM_RPI          // Raspberry Pi
//#define PLATFORM_WEB          // HTML5 (emscripten, asm.js)

// Security check in case no PLATFORM_* defined
#if !defined(PLATFORM_DESKTOP) && !defined(PLATFORM_ANDROID) && !defined(PLATFORM_RPI) && !defined(PLATFORM_WEB)
    #define PLATFORM_DESKTOP
#endif

#if defined(PLATFORM_ANDROID)
    typedef struct android_app;     // Define android_app struct (android_native_app_glue.h)
#endif

//----------------------------------------------------------------------------------
// Some basic Defines
//----------------------------------------------------------------------------------
#ifndef PI
    #define PI 3.14159265358979323846
#endif

#define DEG2RAD (PI/180.0f)
#define RAD2DEG (180.0f/PI)

// raylib Config Flags
#define FLAG_FULLSCREEN_MODE    1
#define FLAG_SHOW_LOGO          2
#define FLAG_SHOW_MOUSE_CURSOR  4
#define FLAG_CENTERED_MODE      8
#define FLAG_MSAA_4X_HINT      16
#define FLAG_VSYNC_HINT        32

// Keyboard Function Keys
#define KEY_SPACE            32
#define KEY_ESCAPE          256
#define KEY_ENTER           257
#define KEY_BACKSPACE       259
#define KEY_RIGHT           262
#define KEY_LEFT            263
#define KEY_DOWN            264
#define KEY_UP              265
#define KEY_F1              290
#define KEY_F2              291
#define KEY_F3              292
#define KEY_F4              293
#define KEY_F5              294
#define KEY_F6              295
#define KEY_F7              296
#define KEY_F8              297
#define KEY_F9              298
#define KEY_F10             299
#define KEY_F11             300
#define KEY_F12             301
#define KEY_LEFT_SHIFT      340
#define KEY_LEFT_CONTROL    341
#define KEY_LEFT_ALT        342
#define KEY_RIGHT_SHIFT     344
#define KEY_RIGHT_CONTROL   345
#define KEY_RIGHT_ALT       346

// Keyboard Alpha Numeric Keys
#define KEY_ZERO             48
#define KEY_ONE              49
#define KEY_TWO              50
#define KEY_THREE            51
#define KEY_FOUR             52
#define KEY_FIVE             53
#define KEY_SIX              54
#define KEY_SEVEN            55
#define KEY_EIGHT            56
#define KEY_NINE             57
#define KEY_A                65
#define KEY_B                66
#define KEY_C                67
#define KEY_D                68
#define KEY_E                69
#define KEY_F                70
#define KEY_G                71
#define KEY_H                72
#define KEY_I                73
#define KEY_J                74
#define KEY_K                75
#define KEY_L                76
#define KEY_M                77
#define KEY_N                78
#define KEY_O                79
#define KEY_P                80
#define KEY_Q                81
#define KEY_R                82
#define KEY_S                83
#define KEY_T                84
#define KEY_U                85
#define KEY_V                86
#define KEY_W                87
#define KEY_X                88
#define KEY_Y                89
#define KEY_Z                90

// Mouse Buttons
#define MOUSE_LEFT_BUTTON     0
#if defined(PLATFORM_WEB)
    #define MOUSE_RIGHT_BUTTON    2
    #define MOUSE_MIDDLE_BUTTON   1
#else
    #define MOUSE_RIGHT_BUTTON    1
    #define MOUSE_MIDDLE_BUTTON   2
#endif

// Touch points registered
#define MAX_TOUCH_POINTS     2

// Gamepad Number
#define GAMEPAD_PLAYER1       0
#define GAMEPAD_PLAYER2       1
#define GAMEPAD_PLAYER3       2
#define GAMEPAD_PLAYER4       3

// Gamepad Buttons
// NOTE: Adjusted for a PS3 USB Controller
#define GAMEPAD_BUTTON_A        2
#define GAMEPAD_BUTTON_B        1
#define GAMEPAD_BUTTON_X        3
#define GAMEPAD_BUTTON_Y        4
#define GAMEPAD_BUTTON_R1       7
#define GAMEPAD_BUTTON_R2       5
#define GAMEPAD_BUTTON_L1       6
#define GAMEPAD_BUTTON_L2       8
#define GAMEPAD_BUTTON_SELECT   9
#define GAMEPAD_BUTTON_START   10

// TODO: Review Xbox360 USB Controller Buttons

// Android Physic Buttons
#define ANDROID_BACK            4
#define ANDROID_MENU            82
#define ANDROID_VOLUME_UP       24
#define ANDROID_VOLUME_DOWN     25

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
#define LIGHTGRAY  (Color){ 200, 200, 200, 255 }   // Light Gray
#define GRAY       (Color){ 130, 130, 130, 255 }   // Gray
#define DARKGRAY   (Color){ 80, 80, 80, 255 }      // Dark Gray
#define YELLOW     (Color){ 253, 249, 0, 255 }     // Yellow
#define GOLD       (Color){ 255, 203, 0, 255 }     // Gold
#define ORANGE     (Color){ 255, 161, 0, 255 }     // Orange
#define PINK       (Color){ 255, 109, 194, 255 }   // Pink
#define RED        (Color){ 230, 41, 55, 255 }     // Red
#define MAROON     (Color){ 190, 33, 55, 255 }     // Maroon
#define GREEN      (Color){ 0, 228, 48, 255 }      // Green
#define LIME       (Color){ 0, 158, 47, 255 }      // Lime
#define DARKGREEN  (Color){ 0, 117, 44, 255 }      // Dark Green
#define SKYBLUE    (Color){ 102, 191, 255, 255 }   // Sky Blue
#define BLUE       (Color){ 0, 121, 241, 255 }     // Blue
#define DARKBLUE   (Color){ 0, 82, 172, 255 }      // Dark Blue
#define PURPLE     (Color){ 200, 122, 255, 255 }   // Purple
#define VIOLET     (Color){ 135, 60, 190, 255 }    // Violet
#define DARKPURPLE (Color){ 112, 31, 126, 255 }    // Dark Purple
#define BEIGE      (Color){ 211, 176, 131, 255 }   // Beige
#define BROWN      (Color){ 127, 106, 79, 255 }    // Brown
#define DARKBROWN  (Color){ 76, 63, 47, 255 }      // Dark Brown

#define WHITE      (Color){ 255, 255, 255, 255 }   // White
#define BLACK      (Color){ 0, 0, 0, 255 }         // Black
#define BLANK      (Color){ 0, 0, 0, 0 }           // Blank (Transparent)
#define MAGENTA    (Color){ 255, 0, 255, 255 }     // Magenta
#define RAYWHITE   (Color){ 245, 245, 245, 255 }   // My own White (raylib logo)

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
#ifndef __cplusplus
// Boolean type
typedef enum { false, true } bool;
#endif

// byte type
typedef unsigned char byte;

// Vector2 type
typedef struct Vector2 {
    float x;
    float y;
} Vector2;

// Vector3 type
typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

// Matrix type (OpenGL style 4x4 - right handed, column major)
typedef struct Matrix {
    float m0, m4, m8, m12;
    float m1, m5, m9, m13;
    float m2, m6, m10, m14;
    float m3, m7, m11, m15;
} Matrix;

// Color type, RGBA (32bit)
typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;

// Rectangle type
typedef struct Rectangle {
    int x;
    int y;
    int width;
    int height;
} Rectangle;

// Image type, bpp always RGBA (32bit)
// NOTE: Data stored in CPU memory (RAM)
typedef struct Image {
    void *data;             // Image raw data
    int width;              // Image base width
    int height;             // Image base height
    int mipmaps;            // Mipmap levels, 1 by default
    int format;             // Data format (TextureFormat)
} Image;

// Texture2D type, bpp always RGBA (32bit)
// NOTE: Data stored in GPU memory
typedef struct Texture2D {
    unsigned int id;        // OpenGL texture id
    int width;              // Texture base width
    int height;             // Texture base height
    int mipmaps;            // Mipmap levels, 1 by default
    int format;             // Data format (TextureFormat)
} Texture2D;

// SpriteFont type, includes texture and charSet array data
typedef struct SpriteFont {
    Texture2D texture;      // Font texture
    int size;               // Base size (default chars height)
    int numChars;           // Number of characters
    int *charValues;        // Characters values array
    Rectangle *charRecs;    // Characters rectangles within the texture
    Vector2 *charOffsets;   // Characters offsets (on drawing)
    int *charAdvanceX;      // Characters x advance (on drawing)
} SpriteFont;

// Camera type, defines a camera position/orientation in 3d space
typedef struct Camera {
    Vector3 position;
    Vector3 target;
    Vector3 up;
} Camera;

// Bounding box type
typedef struct BoundingBox {
    Vector3 min;
    Vector3 max;
} BoundingBox;

// Vertex data definning a mesh
typedef struct Mesh {
    int vertexCount;            // num vertices
    float *vertices;            // vertex position (XYZ - 3 components per vertex)
    float *texcoords;           // vertex texture coordinates (UV - 2 components per vertex)
    float *texcoords2;          // vertex second texture coordinates (useful for lightmaps)
    float *normals;             // vertex normals (XYZ - 3 components per vertex)
    float *tangents;            // vertex tangents (XYZ - 3 components per vertex)
    unsigned char *colors;      // vertex colors (RGBA - 4 components per vertex)
    
    BoundingBox bounds;         // mesh limits defined by min and max points
    
    unsigned int vaoId;         // OpenGL Vertex Array Object id
    unsigned int vboId[6];      // OpenGL Vertex Buffer Objects id (6 types of vertex data)
} Mesh;

// Shader type (generic shader)
typedef struct Shader {
    unsigned int id;                // Shader program id

    // TODO: This should be Texture2D objects
    unsigned int texDiffuseId;      // Diffuse texture id
    unsigned int texNormalId;       // Normal texture id
    unsigned int texSpecularId;     // Specular texture id
    
    // Variable attributes
    int vertexLoc;        // Vertex attribute location point (vertex shader)
    int texcoordLoc;      // Texcoord attribute location point (vertex shader)
    int normalLoc;        // Normal attribute location point (vertex shader)
    int colorLoc;         // Color attibute location point (vertex shader)

    // Uniforms
    int mvpLoc;           // ModelView-Projection matrix uniform location point (vertex shader)
    int tintColorLoc;     // Color uniform location point (fragment shader)
    
    int mapDiffuseLoc;    // Diffuse map texture uniform location point (fragment shader)
    int mapNormalLoc;     // Normal map texture uniform location point (fragment shader)
    int mapSpecularLoc;   // Specular map texture uniform location point (fragment shader)
} Shader;

// Material type
// TODO: Redesign material-shaders-textures system
typedef struct Material {
    //Shader shader;

    //Texture2D texDiffuse;      // Diffuse texture
    //Texture2D texNormal;       // Normal texture
    //Texture2D texSpecular;     // Specular texture
    
    Color colDiffuse;
    Color colAmbient;
    Color colSpecular;
    
    float glossiness;
    float normalDepth;
} Material;

// 3d Model type
// TODO: Replace shader/testure by material
typedef struct Model {
    Mesh mesh;
    Matrix transform;
    Texture2D texture;    // Only for OpenGL 1.1, on newer versions this should be in the shader
    Shader shader;
	//Material material;
} Model;

// Ray type (useful for raycast)
typedef struct Ray {
    Vector3 position;
    Vector3 direction;
} Ray;

// Sound source type
typedef struct Sound {
    unsigned int source;
    unsigned int buffer;
} Sound;

// Wave type, defines audio wave data
typedef struct Wave {
    void *data;                 // Buffer data pointer
    unsigned int dataSize;      // Data size in bytes
    unsigned int sampleRate;
    short bitsPerSample;
    short channels;
} Wave;

// Texture formats
// NOTE: Support depends on OpenGL version and platform
typedef enum {
    UNCOMPRESSED_GRAYSCALE = 1,     // 8 bit per pixel (no alpha)
    UNCOMPRESSED_GRAY_ALPHA,        // 16 bpp (2 channels)
    UNCOMPRESSED_R5G6B5,            // 16 bpp
    UNCOMPRESSED_R8G8B8,            // 24 bpp
    UNCOMPRESSED_R5G5B5A1,          // 16 bpp (1 bit alpha)
    UNCOMPRESSED_R4G4B4A4,          // 16 bpp (4 bit alpha)
    UNCOMPRESSED_R8G8B8A8,          // 32 bpp
    COMPRESSED_DXT1_RGB,            // 4 bpp (no alpha)
    COMPRESSED_DXT1_RGBA,           // 4 bpp (1 bit alpha)
    COMPRESSED_DXT3_RGBA,           // 8 bpp
    COMPRESSED_DXT5_RGBA,           // 8 bpp
    COMPRESSED_ETC1_RGB,            // 4 bpp
    COMPRESSED_ETC2_RGB,            // 4 bpp
    COMPRESSED_ETC2_EAC_RGBA,       // 8 bpp
    COMPRESSED_PVRT_RGB,            // 4 bpp
    COMPRESSED_PVRT_RGBA,           // 4 bpp
    COMPRESSED_ASTC_4x4_RGBA,       // 8 bpp
    COMPRESSED_ASTC_8x8_RGBA        // 2 bpp
} TextureFormat;

// Color blending modes (pre-defined)
typedef enum { BLEND_ALPHA = 0, BLEND_ADDITIVE, BLEND_MULTIPLIED } BlendMode;

// Gestures type
// NOTE: It could be used as flags to enable only some gestures
typedef enum {
    GESTURE_NONE        = 0,
    GESTURE_TAP         = 1,
    GESTURE_DOUBLETAP   = 2,
    GESTURE_HOLD        = 4,
    GESTURE_DRAG        = 8,
    GESTURE_SWIPE_RIGHT = 16,
    GESTURE_SWIPE_LEFT  = 32,
    GESTURE_SWIPE_UP    = 64,
    GESTURE_SWIPE_DOWN  = 128,
    GESTURE_PINCH_IN    = 256,
    GESTURE_PINCH_OUT   = 512
} Gestures;

// Touch action (fingers or mouse)
typedef enum { TOUCH_UP, TOUCH_DOWN, TOUCH_MOVE } TouchAction;

// Gesture events
// NOTE: MAX_TOUCH_POINTS fixed to 2
typedef struct {
    int touchAction;
    int pointCount;
    int pointerId[MAX_TOUCH_POINTS];
    Vector2 position[MAX_TOUCH_POINTS];
} GestureEvent;

// Camera system modes
typedef enum { CAMERA_CUSTOM = 0, CAMERA_FREE, CAMERA_ORBITAL, CAMERA_FIRST_PERSON, CAMERA_THIRD_PERSON } CameraMode;

// Collider types
typedef enum { COLLIDER_CIRCLE, COLLIDER_RECTANGLE, COLLIDER_CAPSULE } ColliderType;

// Transform struct
typedef struct Transform {
    Vector2 position;
    float rotation;
    Vector2 scale;
} Transform;

// Rigidbody struct
typedef struct Rigidbody {
    bool enabled;
    float mass;
    Vector2 acceleration;
    Vector2 velocity;
    bool isGrounded;
    bool isContact;     // Avoid freeze player when touching floor
    bool applyGravity;
    float friction;     // 0.0f to 1.0f
    float bounciness;   // 0.0f to 1.0f
} Rigidbody;

// Collider struct
typedef struct Collider {
    bool enabled;
    ColliderType type;
    Rectangle bounds;   // Used for COLLIDER_RECTANGLE and COLLIDER_CAPSULE
    int radius;         // Used for COLLIDER_CIRCLE and COLLIDER_CAPSULE
} Collider;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//------------------------------------------------------------------------------------
// Global Variables Definition
//------------------------------------------------------------------------------------
// It's lonely here...

//------------------------------------------------------------------------------------
// Window and Graphics Device Functions (Module: core)
//------------------------------------------------------------------------------------
#if defined(PLATFORM_ANDROID)
void InitWindow(int width, int height, struct android_app *state);  // Init Android Activity and OpenGL Graphics
#elif defined(PLATFORM_DESKTOP) || defined(PLATFORM_RPI) || defined(PLATFORM_WEB)
void InitWindow(int width, int height, const char *title);  // Initialize Window and OpenGL Graphics
#endif

void CloseWindow(void);                                     // Close Window and Terminate Context
bool WindowShouldClose(void);                               // Detect if KEY_ESCAPE pressed or Close icon pressed
bool IsWindowMinimized(void);                               // Detect if window has been minimized (or lost focus)
void ToggleFullscreen(void);                                // Fullscreen toggle (only PLATFORM_DESKTOP)
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_RPI)
void SetCustomCursor(const char *cursorImage);              // Set a custom cursor icon/image
void SetExitKey(int key);                                   // Set a custom key to exit program (default is ESC)
#endif
int GetScreenWidth(void);                                   // Get current screen width
int GetScreenHeight(void);                                  // Get current screen height

void ClearBackground(Color color);                          // Sets Background Color
void BeginDrawing(void);                                    // Setup drawing canvas to start drawing
void BeginDrawingEx(int blendMode, Shader shader, Matrix transform);   // Setup drawing canvas with extended parameters
void EndDrawing(void);                                      // End canvas drawing and Swap Buffers (Double Buffering)

void Begin3dMode(Camera camera);                            // Initializes 3D mode for drawing (Camera setup)
void End3dMode(void);                                       // Ends 3D mode and returns to default 2D orthographic mode

Ray GetMouseRay(Vector2 mousePosition, Camera camera);      // Returns a ray trace from mouse position
Vector2 WorldToScreen(Vector3 position, Camera camera);     // Returns the screen space position from a 3d world space position
Matrix GetCameraMatrix(Camera camera);                      // Returns camera transform matrix (view matrix)

void SetTargetFPS(int fps);                                 // Set target FPS (maximum)
float GetFPS(void);                                         // Returns current FPS
float GetFrameTime(void);                                   // Returns time in seconds for one frame

Color GetColor(int hexValue);                               // Returns a Color struct from hexadecimal value
int GetHexValue(Color color);                               // Returns hexadecimal value for a Color
float *ColorToFloat(Color color);                           // Converts Color to float array and normalizes
float *VectorToFloat(Vector3 vec);                          // Converts Vector3 to float array
float *MatrixToFloat(Matrix mat);                           // Converts Matrix to float array

int GetRandomValue(int min, int max);                       // Returns a random value between min and max (both included)
Color Fade(Color color, float alpha);                       // Color fade-in or fade-out, alpha goes from 0.0f to 1.0f

void SetConfigFlags(char flags);                            // Setup some window configuration flags
void ShowLogo(void);                                        // Activates raylib logo at startup (can be done with flags)

bool IsFileDropped(void);                                   // Check if a file have been dropped into window
char **GetDroppedFiles(int *count);                         // Retrieve dropped files into window
void ClearDroppedFiles(void);                               // Clear dropped files paths buffer

void StorageSaveValue(int position, int value);             // Storage save integer value (to defined position)
int StorageLoadValue(int position);                         // Storage load integer value (from defined position)

//------------------------------------------------------------------------------------
// Input Handling Functions (Module: core)
//------------------------------------------------------------------------------------
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_RPI) || defined(PLATFORM_WEB)
bool IsKeyPressed(int key);                             // Detect if a key has been pressed once
bool IsKeyDown(int key);                                // Detect if a key is being pressed
bool IsKeyReleased(int key);                            // Detect if a key has been released once
bool IsKeyUp(int key);                                  // Detect if a key is NOT being pressed
int GetKeyPressed(void);                                // Get latest key pressed

bool IsMouseButtonPressed(int button);                  // Detect if a mouse button has been pressed once
bool IsMouseButtonDown(int button);                     // Detect if a mouse button is being pressed
bool IsMouseButtonReleased(int button);                 // Detect if a mouse button has been released once
bool IsMouseButtonUp(int button);                       // Detect if a mouse button is NOT being pressed
int GetMouseX(void);                                    // Returns mouse position X
int GetMouseY(void);                                    // Returns mouse position Y
Vector2 GetMousePosition(void);                         // Returns mouse position XY
void SetMousePosition(Vector2 position);                // Set mouse position XY
int GetMouseWheelMove(void);                            // Returns mouse wheel movement Y

void ShowCursor(void);                                  // Shows cursor
void HideCursor(void);                                  // Hides cursor
void EnableCursor(void);                                // Enables cursor
void DisableCursor(void);                               // Disables cursor
bool IsCursorHidden(void);                              // Returns true if cursor is not visible

bool IsGamepadAvailable(int gamepad);                   // Detect if a gamepad is available
Vector2 GetGamepadMovement(int gamepad);                // Return axis movement vector for a gamepad
bool IsGamepadButtonPressed(int gamepad, int button);   // Detect if a gamepad button has been pressed once
bool IsGamepadButtonDown(int gamepad, int button);      // Detect if a gamepad button is being pressed
bool IsGamepadButtonReleased(int gamepad, int button);  // Detect if a gamepad button has been released once
bool IsGamepadButtonUp(int gamepad, int button);        // Detect if a gamepad button is NOT being pressed
#endif

int GetTouchX(void);                                    // Returns touch position X for touch point 0 (relative to screen size)
int GetTouchY(void);                                    // Returns touch position Y for touch point 0 (relative to screen size)                   
Vector2 GetTouchPosition(int index);                    // Returns touch position XY for a touch point index (relative to screen size)

#if defined(PLATFORM_ANDROID)
bool IsButtonPressed(int button);                       // Detect if an android physic button has been pressed
bool IsButtonDown(int button);                          // Detect if an android physic button is being pressed
bool IsButtonReleased(int button);                      // Detect if an android physic button has been released
#endif

//------------------------------------------------------------------------------------
// Gestures and Touch Handling Functions (Module: gestures)
//------------------------------------------------------------------------------------
void ProcessGestureEvent(GestureEvent event);           // Process gesture event and translate it into gestures
void UpdateGestures(void);                              // Update gestures detected (must be called every frame)
bool IsGestureDetected(void);                           // Check if a gesture have been detected
int GetGestureType(void);                               // Get latest detected gesture
void SetGesturesEnabled(unsigned int gestureFlags);     // Enable a set of gestures using flags
int GetTouchPointsCount(void);                          // Get touch points count

float GetGestureHoldDuration(void);                     // Get gesture hold time in milliseconds
Vector2 GetGestureDragVector(void);                     // Get gesture drag vector
float GetGestureDragAngle(void);                        // Get gesture drag angle
Vector2 GetGesturePinchVector(void);                    // Get gesture pinch delta
float GetGesturePinchAngle(void);                       // Get gesture pinch angle

//------------------------------------------------------------------------------------
// Camera System Functions (Module: camera)
//------------------------------------------------------------------------------------
void SetCameraMode(int mode);                               // Set camera mode (multiple camera modes available)
void UpdateCamera(Camera *camera);                          // Update camera (player position is ignored)
void UpdateCameraPlayer(Camera *camera, Vector3 *position); // Update camera and player position (1st person and 3rd person cameras)

void SetCameraPosition(Vector3 position);                   // Set internal camera position
void SetCameraTarget(Vector3 target);                       // Set internal camera target

void SetCameraPanControl(int panKey);                       // Set camera pan key to combine with mouse movement (free camera)
void SetCameraAltControl(int altKey);                       // Set camera alt key to combine with mouse movement (free camera)
void SetCameraSmoothZoomControl(int szKey);                 // Set camera smooth zoom key to combine with mouse (free camera)

void SetCameraMoveControls(int frontKey, int backKey, 
                           int leftKey, int rightKey, 
                           int upKey, int downKey);         // Set camera move controls (1st person and 3rd person cameras)
void SetCameraMouseSensitivity(float sensitivity);          // Set camera mouse sensitivity (1st person and 3rd person cameras)

//------------------------------------------------------------------------------------
// Basic Shapes Drawing Functions (Module: shapes)
//------------------------------------------------------------------------------------
void DrawPixel(int posX, int posY, Color color);                                                   // Draw a pixel
void DrawPixelV(Vector2 position, Color color);                                                    // Draw a pixel (Vector version)
void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color);                // Draw a line
void DrawLineV(Vector2 startPos, Vector2 endPos, Color color);                                     // Draw a line (Vector version)
void DrawCircle(int centerX, int centerY, float radius, Color color);                              // Draw a color-filled circle
void DrawCircleGradient(int centerX, int centerY, float radius, Color color1, Color color2);       // Draw a gradient-filled circle
void DrawCircleV(Vector2 center, float radius, Color color);                                       // Draw a color-filled circle (Vector version)
void DrawCircleLines(int centerX, int centerY, float radius, Color color);                         // Draw circle outline
void DrawRectangle(int posX, int posY, int width, int height, Color color);                        // Draw a color-filled rectangle
void DrawRectangleRec(Rectangle rec, Color color);                                                 // Draw a color-filled rectangle
void DrawRectangleGradient(int posX, int posY, int width, int height, Color color1, Color color2); // Draw a gradient-filled rectangle
void DrawRectangleV(Vector2 position, Vector2 size, Color color);                                  // Draw a color-filled rectangle (Vector version)
void DrawRectangleLines(int posX, int posY, int width, int height, Color color);                   // Draw rectangle outline
void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color);                                // Draw a color-filled triangle
void DrawTriangleLines(Vector2 v1, Vector2 v2, Vector2 v3, Color color);                           // Draw triangle outline
void DrawPoly(Vector2 center, int sides, float radius, float rotation, Color color);               // Draw a regular polygon (Vector version)
//void DrawPolyEx(Vector2 *points, int numPoints, Color color);                                      // Draw a closed polygon defined by points
void DrawPolyEx(Vector2 *points, Vector2 center, int numPoints, Color color);
void DrawPolyExLines(Vector2 *points, int numPoints, Color color);                                 // Draw polygon lines

bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2);                                           // Check collision between two rectangles
bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2);        // Check collision between two circles
bool CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec);                         // Check collision between circle and rectangle
Rectangle GetCollisionRec(Rectangle rec1, Rectangle rec2);                                         // Get collision rectangle for two rectangles collision
bool CheckCollisionPointRec(Vector2 point, Rectangle rec);                                         // Check if point is inside rectangle
bool CheckCollisionPointCircle(Vector2 point, Vector2 center, float radius);                       // Check if point is inside circle
bool CheckCollisionPointTriangle(Vector2 point, Vector2 p1, Vector2 p2, Vector2 p3);               // Check if point is inside a triangle

//------------------------------------------------------------------------------------
// Texture Loading and Drawing Functions (Module: textures)
//------------------------------------------------------------------------------------
Image LoadImage(const char *fileName);                                                             // Load an image into CPU memory (RAM)
Image LoadImageEx(Color *pixels, int width, int height);                                           // Load image data from Color array data (RGBA - 32bit)
Image LoadImageRaw(const char *fileName, int width, int height, int format, int headerSize);       // Load image data from RAW file
Image LoadImageFromRES(const char *rresName, int resId);                                           // Load an image from rRES file (raylib Resource)
Texture2D LoadTexture(const char *fileName);                                                       // Load an image as texture into GPU memory
Texture2D LoadTextureEx(void *data, int width, int height, int textureFormat);                     // Load a texture from raw data into GPU memory
Texture2D LoadTextureFromRES(const char *rresName, int resId);                                     // Load an image as texture from rRES file (raylib Resource)
Texture2D LoadTextureFromImage(Image image);                                                       // Load a texture from image data
void UnloadImage(Image image);                                                                     // Unload image from CPU memory (RAM)
void UnloadTexture(Texture2D texture);                                                             // Unload texture from GPU memory
Color *GetImageData(Image image);                                                                  // Get pixel data from image as a Color struct array
Image GetTextureData(Texture2D texture);                                                           // Get pixel data from GPU texture and return an Image
void ImageToPOT(Image *image, Color fillColor);                                                    // Convert image to POT (power-of-two)
void ImageFormat(Image *image, int newFormat);                                                     // Convert image data to desired format
void ImageDither(Image *image, int rBpp, int gBpp, int bBpp, int aBpp);                            // Dither image data to 16bpp or lower (Floyd-Steinberg dithering)
Image ImageCopy(Image image);                                                                      // Create an image duplicate (useful for transformations)
void ImageCrop(Image *image, Rectangle crop);                                                      // Crop an image to a defined rectangle
void ImageResize(Image *image, int newWidth, int newHeight);                                       // Resize and image (bilinear filtering)
void ImageDraw(Image *dst, Image src, Rectangle srcRec, Rectangle dstRec);                         // Draw a source image within a destination image
Image ImageText(const char *text, int fontSize, Color color);                                      // Create an image from text (default font)
Image ImageTextEx(SpriteFont font, const char *text, int fontSize, int spacing, Color tint);       // Create an image from text (custom sprite font)
void ImageFlipVertical(Image *image);                                                              // Flip image vertically
void ImageFlipHorizontal(Image *image);                                                            // Flip image horizontally
void ImageColorTint(Image *image, Color color);                                                    // Modify image color: tint
void ImageColorInvert(Image *image);                                                               // Modify image color: invert
void ImageColorGrayscale(Image *image);                                                            // Modify bimage color: grayscale
void ImageColorContrast(Image *image, float contrast);                                             // Modify image color: contrast (-100 to 100)
void ImageColorBrightness(Image *image, int brightness);                                           // Modify image color: brightness (-255 to 255)
void GenTextureMipmaps(Texture2D texture);                                                         // Generate GPU mipmaps for a texture
void UpdateTexture(Texture2D texture, void *pixels);                                               // Update GPU texture with new data

void DrawTexture(Texture2D texture, int posX, int posY, Color tint);                               // Draw a Texture2D
void DrawTextureV(Texture2D texture, Vector2 position, Color tint);                                // Draw a Texture2D with position defined as Vector2
void DrawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint);  // Draw a Texture2D with extended parameters
void DrawTextureRec(Texture2D texture, Rectangle sourceRec, Vector2 position, Color tint);         // Draw a part of a texture defined by a rectangle
void DrawTexturePro(Texture2D texture, Rectangle sourceRec, Rectangle destRec, Vector2 origin,     // Draw a part of a texture defined by a rectangle with 'pro' parameters
                    float rotation, Color tint);

//------------------------------------------------------------------------------------
// Font Loading and Text Drawing Functions (Module: text)
//------------------------------------------------------------------------------------
SpriteFont GetDefaultFont(void);                                                                   // Get the default SpriteFont
SpriteFont LoadSpriteFont(const char *fileName);                                                   // Load a SpriteFont image into GPU memory
void UnloadSpriteFont(SpriteFont spriteFont);                                                      // Unload SpriteFont from GPU memory

void DrawText(const char *text, int posX, int posY, int fontSize, Color color);                    // Draw text (using default font)
void DrawTextEx(SpriteFont spriteFont, const char* text, Vector2 position,                         // Draw text using SpriteFont and additional parameters
                int fontSize, int spacing, Color tint);
int MeasureText(const char *text, int fontSize);                                                   // Measure string width for default font
Vector2 MeasureTextEx(SpriteFont spriteFont, const char *text, int fontSize, int spacing);         // Measure string size for SpriteFont

void DrawFPS(int posX, int posY);                                                                  // Shows current FPS on top-left corner
const char *FormatText(const char *text, ...);                                                     // Formatting of text with variables to 'embed'
const char *SubText(const char *text, int position, int length);                                   // Get a piece of a text string

//------------------------------------------------------------------------------------
// Basic 3d Shapes Drawing Functions (Module: models)
//------------------------------------------------------------------------------------
void DrawCube(Vector3 position, float width, float height, float lenght, Color color);             // Draw cube
void DrawCubeV(Vector3 position, Vector3 size, Color color);                                       // Draw cube (Vector version)
void DrawCubeWires(Vector3 position, float width, float height, float lenght, Color color);        // Draw cube wires
void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float lenght, Color color); // Draw cube textured
void DrawSphere(Vector3 centerPos, float radius, Color color);                                     // Draw sphere
void DrawSphereEx(Vector3 centerPos, float radius, int rings, int slices, Color color);            // Draw sphere with extended parameters
void DrawSphereWires(Vector3 centerPos, float radius, int rings, int slices, Color color);         // Draw sphere wires
void DrawCylinder(Vector3 position, float radiusTop, float radiusBottom, float height, int slices, Color color); // Draw a cylinder/cone
void DrawCylinderWires(Vector3 position, float radiusTop, float radiusBottom, float height, int slices, Color color); // Draw a cylinder/cone wires
void DrawPlane(Vector3 centerPos, Vector2 size, Color color);                                      // Draw a plane XZ
void DrawQuad(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Color color);                        // Draw a quad
void DrawRay(Ray ray, Color color);                                                                // Draw a ray line
void DrawGrid(int slices, float spacing);                                                          // Draw a grid (centered at (0, 0, 0))
void DrawGizmo(Vector3 position);                                                                  // Draw simple gizmo
//DrawTorus(), DrawTeapot() are useless...

//------------------------------------------------------------------------------------
// Model 3d Loading and Drawing Functions (Module: models)
//------------------------------------------------------------------------------------
Model LoadModel(const char *fileName);                                                             // Load a 3d model (.OBJ)
Model LoadModelEx(Mesh data);                                                                      // Load a 3d model (from vertex data)
//Model LoadModelFromRES(const char *rresName, int resId);                                         // TODO: Load a 3d model from rRES file (raylib Resource)
Model LoadHeightmap(Image heightmap, Vector3 size);                                                // Load a heightmap image as a 3d model
Model LoadCubicmap(Image cubicmap);                                                                // Load a map image as a 3d model (cubes based)
void UnloadModel(Model model);                                                                     // Unload 3d model from memory
void SetModelTexture(Model *model, Texture2D texture);                                             // Link a texture to a model

void DrawModel(Model model, Vector3 position, float scale, Color tint);                            // Draw a model (with texture if set)
void DrawModelEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint);      // Draw a model with extended parameters
void DrawModelWires(Model model, Vector3 position, float scale, Color color);                      // Draw a model wires (with texture if set)
void DrawModelWiresEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint);      // Draw a model wires (with texture if set) with extended parameters
void DrawBoundingBox(BoundingBox box);                                                             // Draw bounding box (wires)

void DrawBillboard(Camera camera, Texture2D texture, Vector3 center, float size, Color tint);                         // Draw a billboard texture
void DrawBillboardRec(Camera camera, Texture2D texture, Rectangle sourceRec, Vector3 center, float size, Color tint); // Draw a billboard texture defined by sourceRec

BoundingBox CalculateBoundingBox(Mesh mesh);                                                       // Calculate mesh bounding box limits
bool CheckCollisionSpheres(Vector3 centerA, float radiusA, Vector3 centerB, float radiusB);                     // Detect collision between two spheres
bool CheckCollisionBoxes(Vector3 minBBox1, Vector3 maxBBox1, Vector3 minBBox2, Vector3 maxBBox2);               // Detect collision between two boxes
bool CheckCollisionBoxSphere(Vector3 minBBox, Vector3 maxBBox, Vector3 centerSphere, float radiusSphere);       // Detect collision between box and sphere
bool CheckCollisionRaySphere(Ray ray, Vector3 spherePosition, float sphereRadius);                              // Detect collision between ray and sphere
bool CheckCollisionRaySphereEx(Ray ray, Vector3 spherePosition, float sphereRadius, Vector3 *collisionPoint);   // Detect collision between ray and sphere with extended parameters and collision point detection
bool CheckCollisionRayBox(Ray ray, Vector3 minBBox, Vector3 maxBBox);                                           // Detect collision between ray and box
Vector3 ResolveCollisionCubicmap(Image cubicmap, Vector3 mapPosition, Vector3 *playerPosition, float radius);   // Detect collision of player radius with cubicmap
                                                                                                                // NOTE: Return the normal vector of the impacted surface
//------------------------------------------------------------------------------------
// Shaders System Functions (Module: rlgl)
// NOTE: This functions are useless when using OpenGL 1.1
//------------------------------------------------------------------------------------
Shader LoadShader(char *vsFileName, char *fsFileName);              // Load a custom shader and bind default locations
unsigned int LoadShaderProgram(char *vShaderStr, char *fShaderStr); // Load custom shaders strings and return program id
void UnloadShader(Shader shader);                                   // Unload a custom shader from memory
void SetPostproShader(Shader shader);                               // Set fullscreen postproduction shader
void SetCustomShader(Shader shader);                                // Set custom shader to be used in batch draw
void SetDefaultShader(void);                                        // Set default shader to be used in batch draw
void SetModelShader(Model *model, Shader shader);                   // Link a shader to a model
bool IsPosproShaderEnabled(void);                                   // Check if postprocessing shader is enabled

int GetShaderLocation(Shader shader, const char *uniformName);                          // Get shader uniform location
void SetShaderValue(Shader shader, int uniformLoc, float *value, int size);             // Set shader uniform value (float)
void SetShaderValuei(Shader shader, int uniformLoc, int *value, int size);              // Set shader uniform value (int)
void SetShaderValueMatrix(Shader shader, int uniformLoc, Matrix mat);                   // Set shader uniform value (matrix 4x4)
void SetShaderMapDiffuse(Shader *shader, Texture2D texture);                            // Default diffuse shader map texture assignment
void SetShaderMapNormal(Shader *shader, const char *uniformName, Texture2D texture);    // Normal map texture shader assignment
void SetShaderMapSpecular(Shader *shader, const char *uniformName, Texture2D texture);  // Specular map texture shader assignment
void SetShaderMap(Shader *shader, int mapLocation, Texture2D texture, int textureUnit); // TODO: Generic shader map assignment

void SetBlendMode(int mode);                                        // Set blending mode (alpha, additive, multiplied)

//----------------------------------------------------------------------------------
// Physics System Functions (engine-module: physac)
//----------------------------------------------------------------------------------
void InitPhysics(int maxPhysicElements);                                    // Initialize all internal physics values
void UnloadPhysics();                                                       // Unload physic elements arrays

void AddRigidbody(int index, Rigidbody rigidbody);                          // Initialize a new rigidbody with parameters to internal index slot
void AddCollider(int index, Collider collider);                             // Initialize a new Collider with parameters to internal index slot

void ApplyPhysics(int index, Vector2 *position);                            // Apply physics to internal rigidbody, physics calculations are applied to position pointer parameter
void SetRigidbodyEnabled(int index, bool state);                            // Set enabled state to a defined rigidbody
void SetRigidbodyVelocity(int index, Vector2 velocity);                     // Set velocity of rigidbody (without considering of mass value)
void SetRigidbodyAcceleration(int index, Vector2 acceleration);             // Set acceleration of rigidbody (without considering of mass value)
void AddRigidbodyForce(int index, Vector2 force);                           // Set rigidbody force (considering mass value)
void AddForceAtPosition(Vector2 position, float intensity, float radius);   // Add a force to all enabled rigidbodies at a position

void SetColliderEnabled(int index, bool state);                             // Set enabled state to a defined collider

Rigidbody GetRigidbody(int index);                                          // Returns the internal rigidbody data defined by index parameter
Collider GetCollider(int index);                                            // Returns the internal collider data defined by index parameter

//------------------------------------------------------------------------------------
// Audio Loading and Playing Functions (Module: audio)
//------------------------------------------------------------------------------------
void InitAudioDevice(void);                                     // Initialize audio device and context
void CloseAudioDevice(void);                                    // Close the audio device and context (and music stream)

Sound LoadSound(char *fileName);                                // Load sound to memory
Sound LoadSoundFromWave(Wave wave);                             // Load sound to memory from wave data
Sound LoadSoundFromRES(const char *rresName, int resId);        // Load sound to memory from rRES file (raylib Resource)
void UnloadSound(Sound sound);                                  // Unload sound
void PlaySound(Sound sound);                                    // Play a sound
void PauseSound(Sound sound);                                   // Pause a sound
void StopSound(Sound sound);                                    // Stop playing a sound
bool SoundIsPlaying(Sound sound);                               // Check if a sound is currently playing
void SetSoundVolume(Sound sound, float volume);                 // Set volume for a sound (1.0 is max level)
void SetSoundPitch(Sound sound, float pitch);                   // Set pitch for a sound (1.0 is base level)

void PlayMusicStream(char *fileName);                           // Start music playing (open stream)
void UpdateMusicStream(void);                                   // Updates buffers for music streaming
void StopMusicStream(void);                                     // Stop music playing (close stream)
void PauseMusicStream(void);                                    // Pause music playing
void ResumeMusicStream(void);                                   // Resume playing paused music
bool MusicIsPlaying(void);                                      // Check if music is playing
void SetMusicVolume(float volume);                              // Set volume for music (1.0 is max level)
float GetMusicTimeLength(void);                                 // Get current music time length (in seconds)
float GetMusicTimePlayed(void);                                 // Get current music time played (in seconds)

#ifdef __cplusplus
}
#endif

#endif // RAYLIB_H
