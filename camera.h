#ifndef CAMERA_H
#define CAMERA_H

#include "vectors.h"

typedef struct {
    Vector3d pos;
    double curX, curY;
    double lastX, lastY;
    double rotX, rotY;
    double zoom;
} Camera;

#endif //CAMERA_H
