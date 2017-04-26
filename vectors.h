#ifndef A1_VELOCITY
#define A1_VELOCITY

typedef struct {
    double x;
    double y;
} vector2d;

typedef struct {
    double x;
    double y;
    double z;
} vector3d;

typedef struct {
    double angle;
    double speed;
} velocity_polar;

typedef vector3d velocity_cartesian;

velocity_cartesian polar_to_cartesian(velocity_polar);

#endif
