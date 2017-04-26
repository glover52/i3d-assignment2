#ifndef A1_FROGGER
#define A1_FROGGER

#include <stdbool.h>
#include "frog.h"

// Handle GLUT events

void display(void);

void animate(void);

void keyboard(unsigned char, int, int);

void special_input(int key, int x, int y);

// Animation

void update_frog_state_analytical(double);

void update_frog_state_numerical(double t);

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
