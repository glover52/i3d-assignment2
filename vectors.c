#include <math.h>
#include "vectors.h"

velocity_cartesian polar_to_cartesian(VelocityPolar polar, double rotation) {
    double magnitude = polar.speed * cos(polar.angle);
    velocity_cartesian result = {
            .x=magnitude * cos(rotation),
            .y=polar.speed * sin(polar.angle),
            .z=magnitude * sin(rotation)
    };
    return result;
}
