//Header for 3D math stuff, see 3dm.c for more info
#ifndef __3DM_H__
#define __3DM_H__

#include <math.h>

//GCC's way of turning a 4D array into something you can perform single operations on
typedef float __attribute__((vector_size(4*sizeof(float)),aligned(sizeof(float)))) vec4;
//A 4D array of the above
typedef vec4 mat4[4];

//Degrees to radians
float d2r(float a);

//Vector generation helpers
vec4 v3f(float a);
vec4 v0(void), v1(void);
vec4 v3(float x, float y, float z);
vec4 v4(float x, float y, float z, float w);

//Some vector functions
vec4 cross(vec4 a, vec4 b);
float dot(vec4 a, vec4 b);
vec4 unit(vec4 a);

//Some matrix generation functions
void idmat(mat4 m);
void persp(mat4 m, float fov, float a, float n, float f);
void look(mat4 m, vec4 f, vec4 t);

//Some matrix manipulation functions
void trans(mat4 m, vec4 p);
void rotx(mat4 m, float a);
void roty(mat4 m, float a);
void rotz(mat4 m, float a);
void scal(mat4 m, vec4 s);

void mult(mat4 m, const mat4 a, const mat4 b);
void invert(mat4 m, const mat4 a);

//vec_transformed = vec * mat, basically
vec4 vmat(vec4 a, const mat4 b);

#endif /* __3DM_H__ */
