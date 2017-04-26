#ifndef A1_FROG
#define A1_FROG

#include "sphere.h"
#include "vectors.h"

typedef struct {
    sphere sphere;
    velocity_cartesian velocity;
    velocity_polar launch_velocity;
    vector3d launch_location;
} frog;

#endif
