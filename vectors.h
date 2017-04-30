#ifndef A1_VELOCITY
#define A1_VELOCITY

typedef struct {
    double x;
    double y;
} Vector2d;

typedef struct {
    double x;
    double y;
    double z;
} Vector3d;

typedef struct {
    double angle;
    double speed;
} VelocityPolar;

typedef Vector3d velocity_cartesian;

velocity_cartesian polar_to_cartesian(VelocityPolar, double rotation);

#endif
