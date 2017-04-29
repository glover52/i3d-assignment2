#ifndef A1_SETTINGS
#define A1_SETTINGS

#include <stdbool.h>

typedef struct {
    bool cartesian;
    bool tangents;
    int segments;
    bool jumping;
    double jump_start_timestamp;
    bool analytical;

    bool axes;
    bool normals;
    bool wireframe;
    bool lighting;
    bool textures;
} Settings;

#endif
