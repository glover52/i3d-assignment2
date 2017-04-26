#ifndef A1_FROG
#define A1_FROG

#include "circle.h"
#include "vectors.h"

typedef struct {
    circle circle;
    velocity_cartesian velocity;
    velocity_polar launch_velocity;
    vector2d launch_location;
} frog;

#endif
