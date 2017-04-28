#include "frogger.h"
#include "settings.h"
#include "camera.h"
#include "frog.h"


Frog frogger = {
        .sphere={.radius=0.05},
        .launch_velocity={1.0, 1.0}
};

Settings mode = {.segments=8 };

Camera camera;

GLuint the_car = 0;
GLuint the_log = 0; 

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

    init();
    glutMainLoop();

    return EXIT_SUCCESS;
}

void init() {
    the_car = create_car();
    the_log = create_log();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

//    glRotated(0.5, 0.0, 1.0, 0);
//    glutPostRedisplay();

    camera_movement();
    draw_grid(8);

    glPushMatrix();
    // Make the camera move with the sphere
//    glTranslated(-camera.pos.x, -camera.pos.y, -camera.pos.z);
//    glRotated(frogger.rotation, 0, 1, 0);
    // Make the camera move with the sphere
    draw_sphere();
    draw_parabola();
    draw_velocity();
    draw_extras(mode.tangents, mode.normals);
    glPopMatrix();

    draw_axes(1.0f);
    draw_obstacles();

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
    camera.rotX = (float) (camera.lastX + x);
    camera.rotY = (float) (camera.lastY + y);
}

void mouseState(int button, int state, int x, int y)
{
    // Button: GLUT_LEFT_BUTTON && GLUT_RIGHT_BUTTON
    // State: GLUT_DOWN && GLUT_UP

    if( button == GLUT_LEFT_BUTTON ) {
        if(state == GLUT_UP) return;

        camera.lastX = x;
        camera.lastY = y;

    } else if( button == GLUT_RIGHT_BUTTON ) {
        if(state == GLUT_UP) return;

        // Input zoom here
//        camera.zoom += 0.01f;
    } else {
        if(state == GLUT_UP) return;

        // Zoom in on scroll up, zoom out on scroll down
        camera.zoom += (button == 3) ? 0.01 : -0.01;
    }
}

void specialKeyboard(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            if(!mode.jumping)
                frogger.rotation += 6;
            break;
        case GLUT_KEY_RIGHT:
            if(!mode.jumping)
                frogger.rotation -= 6;
            break;
        default:
            break;
    }

    glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        /* Toggle axes (should also draw an axes at local origin of every mesh being rendered) */
        case 'o':
            break;

        /* Toggle normals (this should also toggle the tangents for the parabola) */
        case 'n':
            mode.normals = !mode.normals;
            mode.tangents = !mode.tangents;
            printf("Normals: %d, Tangents: %d\n", mode.normals, mode.tangents);
            break;

        /* Toggle wire-frame */
        case 'p':
            break;

        /* Toggle lighting */
        case 'l':
            break;

        /* Toggle textures */
        case 't':
            break;

        case 'w':
            if (frogger.launch_velocity.speed < DBL_MAX)
                frogger.launch_velocity.speed += 0.1;
            break;
        case 's':
            if (frogger.launch_velocity.speed > 0.1)
                frogger.launch_velocity.speed -= 0.1;
            break;
        case 'a':
            if (frogger.launch_velocity.angle < 2.68)
                frogger.launch_velocity.angle += 0.1;
            break;
        case 'd':
            if (frogger.launch_velocity.angle > 0.5)
                frogger.launch_velocity.angle -= 0.1;
            break;
        case ' ':
            if (mode.jumping)
                break;
            frogger.velocity = polar_to_cartesian(frogger.launch_velocity);
            mode.jump_start_timestamp = glutGet(GLUT_ELAPSED_TIME)
                                        / millis_per_sec;
            mode.jumping = true;
            break;

        /* Increase/Decrease amount of segments */
        case '+':
        case '=':
            if (mode.segments < 64)
                mode.segments *= 2;
            break;
        case '-':
        case '_':
            if (mode.segments > 4)
                mode.segments /= 2;
            break;

        /* Toggle between integrations (deprecated) */
        case 'i':
            if (mode.jumping)
                break;
            mode.analytical = !mode.analytical;
            printf("Analytical: %d\n", mode.analytical);
            break;

        /* Exit the program */
        case 27: // [ESC]
        case 'q':
            exit(EXIT_SUCCESS);
        default:
            printf("=== CONTROLS ===\n");
            printf("\'o\': toggle axes\n");
            printf("\'n\': toggle normals\n");
            printf("\'p\': toggle wireframe\n");
            printf("\'l\': toggle lighting\n");
            printf("\'t\': toggle textures\n");
            printf("\'w\': increase speed\n");
            printf("\'s\': decrease speed\n");
            printf("\'a\': increase angle\n");
            printf("\'d\': decrease angle\n");
            printf("\'space\': jump\n");
            printf("\'left mouse\': rotate camera\n");
            printf("\'right mouse\': zoom camera\n");
            printf("\'left arrow\': rotate frog left\n");
            printf("\'right arrow\': rotate frog right\n");
            printf("\'+/-\': double/halve segment amount\n");
    }

    // Request a new frame
    glutPostRedisplay();
}

void update_frog_state_analytical(double time) {
    /* Analytical integration NOT required */
    velocity_cartesian v = polar_to_cartesian(frogger.launch_velocity);
    frogger.sphere.pos.x = v.x * time + frogger.launch_location.x;
    frogger.sphere.pos.y = 0.5 * gravity * pow(time, 2) + v.y * time
                           + frogger.launch_location.y;

    // Modify camera variables to make camera follow frog
    camera.pos.x = -frogger.sphere.pos.x;
    camera.pos.y = -frogger.sphere.pos.y;
    camera.pos.z = -frogger.sphere.pos.z;
}

void update_frog_state_numerical(double dt) {
    /* NUMERICAL INTEGRATION REQUIRED */
    frogger.sphere.pos.x += frogger.velocity.x * dt;
    frogger.sphere.pos.y += frogger.velocity.y * dt;
    frogger.sphere.pos.z += frogger.velocity.z * dt;
    frogger.velocity.y += gravity * dt;

    // Modify camera variables to make camera follow frog
    camera.pos.x = -frogger.sphere.pos.x;
    camera.pos.y = -frogger.sphere.pos.y;
    camera.pos.z = -frogger.sphere.pos.z;
}

void camera_movement() {
    glLoadIdentity();

    // Rotate camera using the X rotation
    glRotated(camera.rotX, 0, 1, 0);

    // Rotate camera using the Y rotation
    glRotated(camera.rotY, 1, 0, 0);

    // Make the camera move with the sphere
    glTranslated(camera.pos.x, camera.pos.y, camera.pos.z);

    // Zoom by translating along the Z axis
    glTranslated(0, 0, camera.zoom);

    // Request a new frame
    glutPostRedisplay();
}

void draw_grid(int n) {
    glPushAttrib(GL_CURRENT_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for (int j = 0; j < n; j++) {
        glBegin(GL_QUAD_STRIP);
        glColor3dv(green);
        build_grid(n, j);
        glEnd();
    }

    glPopAttrib();
}

void draw_sphere() {
    glPushAttrib(GL_CURRENT_BIT);
    double step_phi = M_PI / mode.segments;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (int j = 0; j <= mode.segments; j++) {
        double phi = j / (double)mode.segments * M_PI;
        glBegin(GL_QUAD_STRIP);
        glColor3dv(white);
        build_sphere(phi, step_phi);
        glEnd();
    }

    glPopAttrib();
}

void draw_parabola() {
    glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_LINE_STRIP);
    glColor3dv(yellow);
    build_parabola();
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

void draw_obstacles() {
    glPushAttrib(GL_CURRENT_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    build_obstacles();
    glEnd();
    glPopAttrib();
}

void build_grid(int n, int j) {
    double x, z, xStep, zStep;
    xStep = pow(n, -1) * 2.0;
    zStep = pow(n, -1) * 2.0; // xStep and zStep are the same, but could be different

    for (int i = 0; i <= n; i++) {
        x = -1.0 + i * xStep;
        z = -1.0 + j * zStep;

        glVertex3d(x, 0, z);
        // Replace this comment with calculation to work out next z value
        glVertex3d(x, 0, z + 0.25f);
    }
}

void build_sphere(double phi, double step_phi) {
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

void build_parabola(void) {
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

        Vector3d start = {frogger.sphere.pos.x + x, frogger.sphere.pos.y + y,
        frogger.sphere.pos.z + z};

        if (tangents) {
            Vector3d tangent = {-y, x, z};
            build_vector(start, tangent, 0.1, cyan);
        }
        if (normals) {
            Vector3d normal = {x, y, z};
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

        Vector3d start = {frogger.launch_location.x + x , y, 0};

        if (tangents) {
            Vector3d tangent = {vx, vy, vz};
            build_vector(start, tangent, 0.1, cyan);
        }
        if (normals) {
            Vector3d normal = {-vy, vx, vz};
            build_vector(start, normal, 0.1, yellow);
        }
    }
}

void build_obstacles() {
    for (int i = 0; i < n_logs; i++) {
        glPushMatrix();
        double offset = i * 0.3;
        glTranslated(0.0 + offset, 0.0, 0.2 + offset);
        glRotated(45.0 - i * 25.0, 0.0, 1.0, 0.0);
        glCallList(the_log);
        glPopMatrix();
    }

    for (int i = 0; i < n_cars; i++) {
        glPushMatrix();
        double offset = i * 0.2;
        glTranslated(-0.8 + offset, 0.0, -0.8 + offset);
        glCallList(the_car);
        glPopMatrix();
    }
}

GLuint create_log() {
    GLUquadricObj *quadric = gluNewQuadric();

    GLuint theLog = glGenLists(1);
    glNewList(theLog, GL_COMPILE);

    glShadeModel(GL_FLAT);
    double radius = 0.05;
    double height = 0.2;
    gluCylinder(quadric, radius, radius, height, 8, 8);

    glEndList();

    gluDeleteQuadric(quadric);

    return theLog;
}

GLuint create_car() {
    GLUquadricObj *quadric = gluNewQuadric();

    GLuint theCar = glGenLists(1);
    glNewList(theCar, GL_COMPILE);

    glShadeModel(GL_FLAT);
    glutSolidCube(0.1);

    glEndList();

    gluDeleteQuadric(quadric);

    return theCar;
}

void build_vector(Vector3d p, Vector3d q, double scale, const double *color) {
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
