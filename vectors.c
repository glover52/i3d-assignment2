#include <math.h>
#include "vectors.h"

velocity_cartesian polar_to_cartesian(velocity_polar polar) {
    velocity_cartesian result = {
            .x=polar.speed * cos(polar.angle),
            .y=polar.speed * sin(polar.angle)
    };
    return result;
}
