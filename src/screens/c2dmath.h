/*
*   c2dmath.c
*
*   Simple C 2D maths library made by Marc Montagut - @MarcMDE  
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
*/

#ifndef C2DMATH_H
#define C2DMATH_H

#include "raylib.h"

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

float FloatLerp(float current, float target, float change);
Vector2 Vector2Add(Vector2 a, Vector2 b);
Vector2 Vector2Sub(Vector2 a, Vector2 b);
Vector2 Vector2Product(Vector2 a, Vector2 b);
Vector2 Vector2Quotient(Vector2 a, Vector2 b);
Vector2 Vector2FloatProduct(Vector2 v, float mult);
Vector2 Vector2FloatQuotient(Vector2 v, float div);
void Vector2Scale(Vector2 *v, float scale);
void Vector2Negate(Vector2 *v);
void Vector2Divide(Vector2 *v, float div);
float Vector2DotProduct(Vector2 a, Vector2 b);
float Vector2Lenght(Vector2 v);
void Vector2Normalize(Vector2 *v);
float Vector2Distance(Vector2 a, Vector2 b);
Vector2 Vector2Zero();
Vector2 Vector2Right();
Vector2 Vector2Up();
Vector2 Vector2One();
void Vector2Rotate(Vector2 *v, Vector2 pivot, int angle);
void FloatSwap(float *a, float *b);
Vector2 AngleToVector2(float angle);

#ifdef __cplusplus
}
#endif

#endif // C2DMATH_H