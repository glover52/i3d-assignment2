#ifndef A1_FROGGER
#define A1_FROGGER

#include <stdbool.h>
#include "frog.h"

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

void draw_axes(double);

void draw_circle();

void draw_parabola();

void draw_velocity(void);

void draw_extras(bool tangents, bool normals);

// Build up the required vertexes within the GL context

void build_line(const double *p, const double *q, const double *color);

void build_vector(vector2d p, vector2d q, double scale, const double *color);

void build_circle_parametric(void);

void build_parabola_parametric(void);

void build_circle_extras(bool tangents, bool normals);

void build_parabola_extras(bool tangents, bool normals);

#endif
