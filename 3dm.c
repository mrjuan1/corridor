// This file holds functions for some of the 3D math stuff needed
#include "3dm.h"

#include <string.h>

// Fancy way to declare a 4D array if ints, compiler will attempt to vectorize
// all operations on this type
typedef int
    __attribute__((vector_size(4 * sizeof(int)), aligned(sizeof(int)))) v4i;

#define shfl __builtin_shuffle

const float _pi180 = M_PI / 180.0f;

// Reference indices for calculating cross products
const v4i _ci[2] = {{1, 2, 0, 3}, {2, 0, 1, 3}};

// Identity matrix
const mat4 _idmat = {{1.0f, 0.0f, 0.0f, 0.0f},
                     {0.0f, 1.0f, 0.0f, 0.0f},
                     {0.0f, 0.0f, 1.0f, 0.0f},
                     {0.0f, 0.0f, 0.0f, 1.0f}};
const int _mat4s = sizeof(mat4);

// const up vector for lookat function
const vec4 _up = {0.0f, 0.0f, 1.0f, 1.0f};

// Index list for 4x4 matrix inversion
// Mostly Google'd stuff a long time ago combined to make a fast-ish matrix
// inversion function, I don't rmember much about it
const v4i _ii[6] = {{0, 1, 4, 5}, {0, 2, 4, 6}, {1, 3, 5, 7},
                    {2, 3, 6, 7}, {1, 0, 3, 2}, {2, 3, 0, 1}};

// Degrees to radians
float d2r(float a) { return a * _pi180; }

// Next few functions were shortcuts for vector-generation
vec4 v3f(float a) {
  const vec4 t = {a, a, a, 1.0f};
  return t;
}

vec4 v0(void) { return v3f(0.0f); }

vec4 v1(void) { return v3f(1.0f); }

vec4 v3(float x, float y, float z) {
  const vec4 t = {x, y, z, 1.0f};
  return t;
}

vec4 v4(float x, float y, float z, float w) {
  const vec4 t = {x, y, z, w};
  return t;
}

vec4 cross(vec4 a, vec4 b) {
  return (shfl(a, _ci[0]) * shfl(b, _ci[1])) -
         (shfl(a, _ci[1]) * shfl(b, _ci[0]));
}

float dot(vec4 a, vec4 b) {
  const vec4 t = a * b;
  return t[0] + t[1] + t[2] + t[3];
}

// Normalize, can't remember why I decided to call this "unit"
vec4 unit(vec4 a) {
  const float t = sqrtf(dot(a, a));
  if (t != 0.0f)
    return a * (1.0f / t);
  else
    return v0();
}

void idmat(mat4 m) { memcpy((void *)m, (const void *)_idmat, _mat4s); }

// Perspective projection matrix generator
void persp(mat4 m, float fov, float a, float n, float f) {
  const float nf = 1.0f / (n - f);

  memset((void *)m, 0, _mat4s);

  m[1][1] = 1.0f / tanf(d2r(fov / 2.0f));
  m[0][0] = m[1][1] / a;
  m[2][2] = (n + f) * nf;
  m[2][3] = -1.0f;
  m[3][2] = 2.0f * n * f * nf;
}

// Look-at view matrix generator
void look(mat4 m, vec4 f, vec4 t) {
  const vec4 z = unit(f - t);
  const vec4 x = unit(cross(_up, z));
  const vec4 y = unit(cross(z, x));
  int i = 0;

  memset((void *)m, 0, _mat4s);

  for (i = 0; i < 3; i++) {
    m[i][0] = x[i];
    m[i][1] = y[i];
    m[i][2] = z[i];
  }

  m[3][0] = -dot(x, f);
  m[3][1] = -dot(y, f);
  m[3][2] = -dot(z, f);
  m[3][3] = 1.0f;
}

// Translate
void trans(mat4 m, vec4 p) {
  m[3] = (m[0] * p[0]) + (m[1] * p[1]) + (m[2] * p[2]) + (m[3] * p[3]);
}

// Next 3 functions are for rotation
void rotx(mat4 m, float a) {
  const float r = d2r(a), c = cosf(r), s = sinf(r);
  vec4 t[2];
  t[0] = m[1];
  t[1] = m[2];

  m[1] = (t[0] * c) + (t[1] * -s);
  m[2] = (t[0] * s) + (t[1] * c);
}

void roty(mat4 m, float a) {
  const float r = d2r(a), c = cosf(r), s = sinf(r);
  vec4 t[2];
  t[0] = m[0];
  t[1] = m[2];

  m[0] = (t[0] * c) + (t[1] * -s);
  m[2] = (t[0] * s) + (t[1] * c);
}

void rotz(mat4 m, float a) {
  const float r = d2r(a), c = cosf(r), s = sinf(r);
  vec4 t[2];
  t[0] = m[0];
  t[1] = m[1];

  m[0] = (t[0] * c) + (t[1] * -s);
  m[1] = (t[0] * s) + (t[1] * c);
}

// Scaling
void scal(mat4 m, vec4 s) {
  int i = 0;
  for (i = 0; i < 4; i++)
    m[i] *= s[i];
}

// Multiplying two 4x4 matrices
void mult(mat4 m, const mat4 a, const mat4 b) {
  int i = 0;
  for (i = 0; i < 4; i++)
    m[i] = (a[0] * b[i][0]) + (a[1] * b[i][1]) + (a[2] * b[i][2]) +
           (a[3] * b[i][3]);
}

// Matrix inversion, as mentioned before, this mess was an effort of combined
// Google searches and long-forgotten experiments that resulted in a
// quicker-than-usual matrix inversion function
void invert(mat4 m, const mat4 a) {
  vec4 t1, t3;
  mat4 t2;

  t1 = shfl(a[0], a[1], _ii[0]);
  t2[1] = shfl(a[2], a[3], _ii[0]);
  t2[0] = shfl(t1, t2[1], _ii[1]);
  t2[1] = shfl(t2[1], t1, _ii[2]);
  t1 = shfl(a[0], a[1], _ii[3]);
  t2[3] = shfl(a[2], a[3], _ii[3]);
  t2[2] = shfl(t1, t2[3], _ii[1]);
  t2[3] = shfl(t2[3], t1, _ii[2]);

  t1 = shfl(t2[2] * t2[3], _ii[4]);
  m[0] = t2[1] * t1;
  m[1] = t2[0] * t1;
  t1 = shfl(t1, _ii[5]);
  m[0] = (t2[1] * t1) - m[0];
  m[1] = (t2[0] * t1) - m[1];
  m[1] = shfl(m[1], _ii[5]);

  t1 = shfl(t2[1] * t2[2], _ii[4]);
  m[0] += t2[3] * t1;
  m[3] = t2[0] * t1;
  t1 = shfl(t1, _ii[5]);
  m[0] -= t2[3] * t1;
  m[3] = (t2[0] * t1) - m[3];
  m[3] = shfl(m[3], _ii[5]);

  t1 = shfl(shfl(t2[1], _ii[5]) * t2[3], _ii[4]);
  t2[2] = shfl(t2[2], _ii[5]);
  m[0] += t2[2] * t1;
  m[2] = t2[0] * t1;
  t1 = shfl(t1, _ii[5]);
  m[0] -= t2[2] * t1;
  m[2] = (t2[0] * t1) - m[2];
  m[2] = shfl(m[2], _ii[5]);

  t1 = shfl(t2[0] * t2[1], _ii[4]);
  m[2] += t2[3] * t1;
  m[3] = (t2[2] * t1) - m[3];
  t1 = shfl(t1, _ii[5]);
  m[2] = (t2[3] * t1) - m[2];
  m[3] -= t2[2] * t1;

  t1 = shfl(t2[0] * t2[3], _ii[4]);
  m[1] -= t2[2] * t1;
  m[2] += t2[1] * t1;
  t1 = shfl(t1, _ii[5]);
  m[1] += t2[2] * t1;
  m[2] -= t2[1] * t1;

  t1 = shfl(t2[0] * t2[2], _ii[4]);
  m[1] += t2[3] * t1;
  m[3] -= t2[1] * t1;
  t1 = shfl(t1, _ii[5]);
  m[1] -= t2[3] * t1;
  m[3] += t2[1] * t1;

  t3 = t2[0] * m[0];
  t3 += shfl(t3, _ii[5]);
  t3 += shfl(t3, _ii[4]);

  if (t3[0] != 0.0f) {
    const float d = 1.0f / t3[0];
    int i = 0;

    for (i = 0; i < 4; i++)
      m[i] *= d;
  } else
    idmat(m);
}

// Multiplies a vector my a matrix and returns a vector
vec4 vmat(vec4 a, const mat4 b) {
  const vec4 t = (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]) + (a[3] * b[3]);
  if (t[3] != 0.0f)
    return t * (1.0f / t[3]);
  else
    return v0();
}
