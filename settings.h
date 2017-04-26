#ifndef A1_SETTINGS
#define A1_SETTINGS

#include <stdbool.h>

typedef struct {
    bool cartesian;
    bool normals;
    bool tangents;
    int segments;
    bool jumping;
    double jump_start_timestamp;
    bool analytical;
} settings;

#endif
