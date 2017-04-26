#include "frogger.h"


frog frogger = {
        .sphere={.radius=0.05},
        .launch_velocity={1.0, 1.0}
};

settings mode = {.segments=8 };

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(W_WIDTH, W_HEIGHT);
    glutInitWindowPosition(W_DEFAULT, W_DEFAULT);
    glutCreateWindow(W_NAME);

    glMatrixMode(GL_PROJECTION);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    // Mouse commands
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouseState);

    // Keyboard commands
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);

    glutDisplayFunc(display);
    glutIdleFunc(animate);

    glutMainLoop();

    return EXIT_SUCCESS;
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
//    glLoadIdentity();

    glRotated(0.5, 0.0, 1.0, 0);
    glutPostRedisplay();

    draw_sphere();
    draw_parabola();
    draw_velocity();
    draw_extras(mode.tangents, mode.normals);
    draw_axes(1.0f);

    glutSwapBuffers();
}

void animate() {
    if (!mode.jumping)
        return;

    static double previous_timestamp = -1.0;
    double timestamp = (glutGet(GLUT_ELAPSED_TIME) / millis_per_sec)
                       - mode.jump_start_timestamp;

    if (previous_timestamp < 0.0) {
        previous_timestamp = timestamp;
        return;
    }

    double delta_time = timestamp - previous_timestamp;

    if (mode.analytical)
        update_frog_state_analytical(timestamp);

    else
        update_frog_state_numerical(delta_time);

    previous_timestamp = timestamp;

    if (frogger.sphere.pos.y < 0.0) {
        mode.jumping = false;
        frogger.sphere.pos.y = 0.0;
        frogger.launch_location = frogger.sphere.pos;
        previous_timestamp = -1.0;
    }

    glutPostRedisplay();
}

void mouseMotion(int x, int y)
{
    // Called when left or right mouse button pressed AND mouse moved

}

void mouseState(int button, int state, int x, int y)
{
    // Button: GLUT_LEFT_BUTTON && GLUT_RIGHT_BUTTON
    // State: GLUT_DOWN && GLUT_UP
}

void specialKeyboard(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            if (mode.segments < INT_MAX / 2)
                mode.segments *= 2;
            break;
        case GLUT_KEY_DOWN:
            if (mode.segments > 2)
                mode.segments /= 2;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            if (frogger.launch_velocity.speed < DBL_MAX) {
                frogger.launch_velocity.speed += 0.1;
            }
            break;
        case 's':
            if (frogger.launch_velocity.speed > 0.1) {
                frogger.launch_velocity.speed -= 0.1;
            };
            break;
        case 'a':
            if (frogger.launch_velocity.angle < 2)
                frogger.launch_velocity.angle += 0.1;
            break;
        case 'd':
            if (frogger.launch_velocity.angle > 0.1) {
                frogger.launch_velocity.angle -= 0.1;
            }
            break;
        case 't':
            mode.tangents = !mode.tangents;
            printf("");
            break;
        case 'n':
            mode.normals = !mode.normals;
            break;
        case 'f':
            break;
        case 'i':
            if (!mode.jumping) {
                mode.analytical = !mode.analytical;
                break;
            }
        case ' ':
            if (!mode.jumping) {
                frogger.velocity = polar_to_cartesian(frogger.launch_velocity);
                mode.jump_start_timestamp = glutGet(GLUT_ELAPSED_TIME)
                                            / millis_per_sec;
                mode.jumping = true;
            }
            break;
        case 27: // [ESC]
        case 'q':
            exit(EXIT_SUCCESS);
        default:
            printf("=== CONTROLS ===\n");
            printf("\'w\': increase speed\n");
            printf("\'s\': decrease speed\n");
            printf("\'a\': increase angle\n");
            printf("\'d\': decrease angle\n");
            printf("\'t\': toggle tangent visuals\n");
            printf("\'n\': toggle normal visuals\n");
            printf("\'f\': none\n");
            printf("\'i\': toggle between integrations\n");
            printf("\'up\': double segment amoutn\n");
            printf("\'down\': half segment amoutn\n");
            printf("\'space\': jump!\n\n");
    }

    // Request a new frame
    glutPostRedisplay();
}

void update_frog_state_analytical(double time) {
    velocity_cartesian v = polar_to_cartesian(frogger.launch_velocity);
    frogger.sphere.pos.x = v.x * time + frogger.launch_location.x;
    frogger.sphere.pos.y = 0.5 * gravity * pow(time, 2) + v.y * time
                           + frogger.launch_location.y;
}

void update_frog_state_numerical(double dt) {
    frogger.sphere.pos.x += frogger.velocity.x * dt;
    frogger.sphere.pos.y += frogger.velocity.y * dt;
    frogger.velocity.y += gravity * dt;
}

void draw_sphere() {
    double step_phi = M_PI / mode.segments;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (int j = 0; j <= mode.segments; j++) {
        double phi = j / (double)mode.segments * M_PI;
        glBegin(GL_QUAD_STRIP);
        build_sphere_parametric(phi, step_phi);
        glEnd();
    }
}

void draw_parabola() {
    glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_LINE_STRIP);
    glColor3dv(yellow);
    build_parabola_parametric();
    glEnd();
    glPopAttrib();
}

void draw_velocity() {
    glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_LINES);
    velocity_cartesian v = polar_to_cartesian(frogger.launch_velocity);
    build_vector(frogger.launch_location, v, 0.2, magenta);
    glEnd();
    glPopAttrib();
}

void draw_extras(bool tangents, bool normals) {
    if (!tangents && !normals)
        return;

    glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_LINES);
    build_circle_extras(tangents, normals);
    build_parabola_extras(tangents, normals);
    glEnd();
    glPopAttrib();
}

void draw_axes(double length) {
    double x_axis[] = {length, 0.0, 0.0};
    double y_axis[] = {0.0, length, 0.0};
    double z_axis[] = {0.0, 0.0, length};

    glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_LINES);
    build_line(origin, x_axis, red);
    build_line(origin, y_axis, green);
    build_line(origin, z_axis, blue);
    glEnd();
    glPopAttrib();
}

void build_sphere_parametric(double phi, double step_phi) {
    int slices = mode.segments;
    double r = frogger.sphere.radius;

    for (int i = 0; i <= slices; i++) {
        double theta = i / (float)slices * 2.0 * M_PI;
        double x1 = frogger.sphere.pos.x + r * sin(phi) * cos(theta);
        double y1 = frogger.sphere.pos.y + r * sin(phi) * sin(theta);
        double z1 = frogger.sphere.pos.z + r * cos(phi);
        double x2 = frogger.sphere.pos.x + r * sin(phi + step_phi) * cos(theta);
        double y2 = frogger.sphere.pos.y + r * sin(phi + step_phi) * sin(theta);
        double z2 = frogger.sphere.pos.z + r * cos(phi + step_phi);
        glVertex3d(x1, y1, z1);
        glVertex3d(x2, y2, z2);
    }
}

void build_parabola_parametric(void) {
    double distance = (-1.0 / gravity) * 2 * frogger.launch_velocity.speed
                      * sin(frogger.launch_velocity.angle);
    double step = distance / mode.segments;

    for (int i = 0; i <= mode.segments; i++) {
        double t = i * step;

        double x = frogger.launch_velocity.speed * t
                   * cos(frogger.launch_velocity.angle);

        double y = frogger.launch_velocity.speed * t
                   * sin(frogger.launch_velocity.angle)
                   + 0.5f * gravity * t * t;

        glVertex3d(x + frogger.launch_location.x, y, 0);
    }
}

void build_circle_extras(bool tangents, bool normals) {
    double step = (2 * M_PI) / mode.segments;
    for (int i = 0; i < mode.segments; i++) {
        double theta = i * step;
        double x = frogger.sphere.radius * cos(theta);
        double y = frogger.sphere.radius * sin(theta);
        double z = 0.0;
        vector3d start = {frogger.sphere.pos.x + x, frogger.sphere.pos.y + y,
        frogger.sphere.pos.z + z};

        if (tangents) {
            vector3d tangent = {-y, x, z};
            build_vector(start, tangent, 0.1, cyan);
        }
        if (normals) {
            vector3d normal = {x, y, z};
            build_vector(start, normal, 0.1, yellow);
        }
    }
}

void build_parabola_extras(bool tangents, bool normals) {
    double distance = (-1.0 / gravity) * 2 * frogger.launch_velocity.speed
                      * sin(frogger.launch_velocity.angle);
    double step = distance / mode.segments;

    velocity_cartesian v0 = polar_to_cartesian(frogger.launch_velocity);

    for (int i = 0; i <= mode.segments; i++) {
        double t = i * step;

        double vx = v0.x;
        double vy = v0.y + gravity * t;
        double vz = 0.0;

        double x = frogger.launch_velocity.speed * t
                   * cos(frogger.launch_velocity.angle);
        double y = frogger.launch_velocity.speed * t
                   * sin(frogger.launch_velocity.angle)
                   + 0.5 * gravity * t * t;
        double z = 0.0;

        vector3d start = {frogger.launch_location.x + x , y, z};

        if (tangents) {
            vector3d tangent = {vx, vy, vz};
            build_vector(start, tangent, 0.1, cyan);
        }
        if (normals) {
            vector3d normal = {-vy, vx, vz};
            build_vector(start, normal, 0.1, yellow);
        }
    }
}

void build_vector(vector3d p, vector3d q, double scale, const double *color) {
    double magnitude = sqrt( pow(q.x,2) * pow(q.y,2) + pow(q.z,2) );
    double x = (q.x / magnitude) * scale;
    double y = (q.y / magnitude) * scale;
    double z = (q.z / magnitude) * scale;

    double start[] = {p.x, p.y, p.z};
    double end[] = {p.x + x, p.y + y, p.z + z};
    build_line(start, end, color);
}


void build_line(const double *p, const double *q, const double *color) {
    glColor3dv(color);
    glVertex3dv(p);
    glVertex3dv(q);
}
