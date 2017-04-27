#ifndef A1_FROG
#define A1_FROG

#include "sphere.h"
#include "vectors.h"

typedef struct {
    Sphere sphere;
    velocity_cartesian velocity;
    VelocityPolar launch_velocity;
    Vector3d launch_location;
} Frog;

#endif
