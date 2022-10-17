#ifndef PARTICLE_H
#define PARTICLE_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

EXTERNC void initParticle();
EXTERNC void addParticle(float x, float y, float count, float speed,
                         float angle, float angleWidth);
EXTERNC void updateParticles();

#endif
