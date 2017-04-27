#ifndef A1_FROGGER
#define A1_FROGGER

#if _WIN32
#   include <Windows.h>
#endif

#if __APPLE__
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#   include <GLUT/glut.h>
#else
#   include <GL/gl.h>
#   include <GL/glu.h>
#   include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>

#include "invariants.h"
#include "vectors.h"
#include "frog.h"
#include "colours.h"
#include "settings.h"

#define W_NAME "COSC1187 Assignment 2 - Frogger"
#define W_WIDTH 640
#define W_HEIGHT 480
#define W_DEFAULT 10

// Handle GLUT events

void display(void);

void animate(void);

void mouseMotion(int, int);

void mouseState(int, int, int, int);

void keyboard(unsigned char, int, int);

void specialKeyboard(int, int, int);

// Animation

void update_frog_state_analytical(double);

void update_frog_state_numerical(double);

// Open a GL context to draw objects

void camera_movement();

void draw_axes(double);

void draw_sphere();

void draw_parabola();

void draw_velocity(void);

void draw_extras(bool tangents, bool normals);

// Build up the required vertexes within the GL context

void build_line(const double *p, const double *q, const double *color);

void build_vector(Vector3d p, Vector3d q, double scale, const double *color);

void build_sphere(double, double);

void build_parabola(void);

void build_circle_extras(bool tangents, bool normals);

void build_parabola_extras(bool tangents, bool normals);

#endif
