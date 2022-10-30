/// \cond
#ifndef VECTOR_H
#define VECTOR_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

typedef struct {
  float x;
  float y;
} Vector;

EXTERNC Vector* vectorSet(Vector* vec, float x, float y);
EXTERNC Vector* vectorAdd(Vector* vec, float x, float y);
EXTERNC Vector* vectorMul(Vector* vec, float v);
EXTERNC Vector* rotate(Vector* vec, float angle);
EXTERNC Vector* addWithAngle(Vector* vec, float angle, float length);
EXTERNC float angleTo(Vector* vec, float x, float y);
EXTERNC float distanceTo(Vector* vec, float x, float y);
EXTERNC float vectorAngle(Vector* vec);
EXTERNC float vectorLength(Vector* vec);
/// \endcond

//! Expand 'v' to 'v.x, v.y'
#define VEC_XY(v) v.x, v.y

#endif
