#include "frogger.h"
#include "settings.h"

Frog frogger = {
        .sphere={.radius=0.05},
        .launch_velocity={1.0, 1.0}
};

Settings mode = {.segments=8, .axes=true, .wireframe=true};

Camera camera;

GLfloat light_position[] = { -1, 1, 1, 0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 100.0 };

GLuint the_car = 0;
GLuint the_log = 0; 

double logs[N_LOGS] = { 0 };
double cars[N_CARS] = { 0 };

GLuint log_texture, frog_texture, grass_texture, rock_texture;

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
    frog_texture = loadTexture("textures/frog.jpg");
    if(!frog_texture) printf("Frog texture failed to load!\n");

    log_texture = loadTexture("textures/wood.jpg");
    if(!log_texture) printf("Log texture failed to load!\n");

    grass_texture = loadTexture("textures/grass.jpg");
    if(!grass_texture) printf("Grass texture failed to load!\n");

    rock_texture = loadTexture("textures/rock.jpg");
    if(!rock_texture) printf("Rock texture failed to load!\n");

    srand( time(NULL) );

    the_car = create_car();
    the_log = create_log();

    // Initial offset for each car / log
    for (int i = 0; i < N_LOGS; i++)
        logs[i] -= rand();

    for (int i = 0; i < N_CARS; i++)
        cars[i] -= rand();

    Vector3d pos = {0, 0, -1};
    frogger.sphere.pos = pos;
    frogger.launch_location = pos;
}

void display() {
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, log_texture);
//
//    glBegin(GL_QUADS);
//    glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
//    glTexCoord2f(1, 0); glVertex3f(1, -1, 0);
//    glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
//    glTexCoord2f(0, 1); glVertex3f(-1, 1, 0);
//    glEnd();


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if(mode.lighting) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    } else {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }

    if(mode.textures) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_TEXTURE_2D);
//        glPushAttrib(GL_TEXTURE_BIT);
//        glBindTexture(GL_TEXTURE_2D, log_texture);
//
//        glBegin(GL_QUADS);
//        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
//        glTexCoord2f(1, 0); glVertex3f(1, -1, 0);
//        glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
//        glTexCoord2f(0, 1); glVertex3f(-1, 1, 0);
//        glEnd();
//        glPopAttrib();
    } else
        glDisable(GL_TEXTURE_2D);

//    glRotated(0.5, 0.0, 1.0, 0);
//    glutPostRedisplay();

    camera_movement();
    draw_axes(1.0f);
    draw_sphere();
    draw_parabola();
    draw_velocity();
    draw_obstacles();
    draw_grid(8);
    draw_extras(mode.tangents, mode.normals);

    glutSwapBuffers();
}

void animate() {
    static double previous_timestamp = -1.0;
    double timestamp = (glutGet(GLUT_ELAPSED_TIME) / millis_per_sec);

    if (previous_timestamp < 0.0) {
        previous_timestamp = timestamp;
        return;
    }

    double delta_time = timestamp - previous_timestamp;

    if (mode.jumping) {
        update_frog_state_numerical(delta_time);
    }

    previous_timestamp = timestamp;

    if (mode.jumping && frogger.sphere.pos.y < 0.0) {
        mode.jumping = false;
        frogger.sphere.pos.y = 0.0;
        frogger.launch_location = frogger.sphere.pos;
        previous_timestamp = -1.0;
    }

    // Update logs and cars
    for (int i = 0; i < N_LOGS; i++) {
        logs[i] += delta_time / 2.0;
        logs[i] = fmod(logs[i], 2.0);
    }

    for (int i = 0; i < N_LOGS; i++) {
        cars[i] += delta_time / 2.0;
        cars[i] = fmod(cars[i], 2.0);
    }


    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    // Called when left or right mouse button pressed AND mouse moved
    camera.rotX = (float) (camera.lastX + x);
    camera.rotY = (float) (camera.lastY + y);
}

void mouseState(int button, int state, int x, int y) {
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
            mode.axes = !mode.axes;
            printf("Axes: %s\n", (mode.axes) ? "on" : "off");
            break;

        /* Toggle normals (this should also toggle the tangents for the parabola) */
        case 'n':
            mode.normals = !mode.normals;
            mode.tangents = !mode.tangents;
            printf("Normals: %s, Tangents %s\n",
                   (mode.normals) ? "on" : "off",
                   (mode.tangents) ? "on" : "off");
            break;

        /* Toggle wireframe */
        case 'p':
            mode.wireframe = !mode.wireframe;
            printf("Wireframe: %s\n", (mode.wireframe) ? "on" : "off");
            break;

        /* Toggle lighting */
        case 'l':
            mode.lighting = !mode.lighting;
            printf("Lighting: %s\n", (mode.lighting) ? "on" : "off");
            break;

        /* Toggle textures */
        case 't':
            mode.textures = !mode.textures;
            printf("Textures: %s\n", (mode.textures) ? "on" : "off");
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

        /* Exit the program */
        case 27: // [ESC]
        case 'q':
            exit(EXIT_SUCCESS);
        default:
            printf("\n=== CONTROLS ===\n");
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
    glPushMatrix();

    if(mode.wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if(mode.lighting) {
        glPushAttrib(GL_LIGHTING_BIT);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green_float);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    } else {
        glPushAttrib(GL_CURRENT_BIT);
        glColor3dv(green);
    }

    if(mode.textures) {
        glPushAttrib(GL_TEXTURE_BIT);
        glBindTexture(GL_TEXTURE_2D, frog_texture);
    }

    for (int i = 0; i < n; ++i) {
        glBegin(GL_QUAD_STRIP);
        build_grid(n, i);
        glEnd();
    }

    if(mode.textures) glPopAttrib();
    glPopAttrib();

    glPopMatrix();
}

void draw_sphere() {
    glPushMatrix();

    if(mode.wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if(!mode.lighting) {
        glPushAttrib(GL_CURRENT_BIT);
        glColor3dv(white);
    } else {
        glPushAttrib(GL_LIGHTING_BIT);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white_float);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    }

    if(mode.textures) {
        glPushAttrib(GL_TEXTURE_BIT);
        glBindTexture(GL_TEXTURE_2D, frog_texture);
    }

    double step_phi = M_PI / mode.segments;
    for (int j = 0; j <= mode.segments; j++) {
        double phi = j / (double)mode.segments * M_PI;
        glBegin(GL_QUAD_STRIP);
        build_sphere(phi, step_phi);
        glEnd();
    }

    if(mode.textures) glPopAttrib();
    glPopAttrib();
    glPopMatrix();
}

void draw_parabola() {
    if(mode.lighting) glDisable(GL_LIGHTING);
    if(mode.textures) glDisable(GL_TEXTURE_2D);

    glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_LINE_STRIP);
    glColor3dv(yellow);
    build_parabola();
    glEnd();
    glPopAttrib();
}

void draw_velocity() {
    if(mode.lighting) glDisable(GL_LIGHTING);
    if(mode.textures) glDisable(GL_TEXTURE_2D);

    glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_LINES);
    velocity_cartesian v = polar_to_cartesian(frogger.launch_velocity);
    build_vector(frogger.launch_location, v, 0.2, magenta);
    glEnd();
    glPopAttrib();

    if(mode.textures) glEnable(GL_TEXTURE_2D);
    if(mode.lighting) glEnable(GL_LIGHTING);
}

void draw_extras(bool tangents, bool normals) {
    if (!tangents && !normals) return;

    if(mode.lighting) glDisable(GL_LIGHTING);
    if(mode.textures) glDisable(GL_TEXTURE_2D);

    glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_LINES);
    build_circle_extras(tangents, normals);
    build_parabola_extras(tangents, normals);
    build_grid_extras(normals);
    glEnd();
    glPopAttrib();

    if(mode.textures) glEnable(GL_TEXTURE_2D);
    if(mode.lighting) glEnable(GL_LIGHTING);
}

void draw_axes(double length) {
    if (!mode.axes) return;

    double x_axis[] = {length, 0.0, 0.0};
    double y_axis[] = {0.0, length, 0.0};
    double z_axis[] = {0.0, 0.0, length};

    if(mode.lighting) glDisable(GL_LIGHTING);
    if(mode.textures) glDisable(GL_TEXTURE_2D);

    glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_LINES);
    build_line(origin, x_axis, red);
    build_line(origin, y_axis, green);
    build_line(origin, z_axis, blue);
    glEnd();
    glPopAttrib();

    if(mode.textures) glDisable(GL_TEXTURE_2D);
    if(mode.lighting) glEnable(GL_LIGHTING);
}

void draw_obstacles() {
    if(mode.wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if(!mode.lighting) {
        glPushAttrib(GL_CURRENT_BIT);
        glColor3dv(facebook_blue);
        glPushAttrib(GL_CURRENT_BIT);
        glColor3dv(brown);
    } else {
        glPushAttrib(GL_LIGHTING_BIT);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, facebook_float);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

        glPushAttrib(GL_LIGHTING_BIT);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brown_float);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    }

    build_obstacles();
    glEnd();
}

void build_grid(int n, int i) {
    double x, z, xStep, zStep;
    xStep = pow(n, -1) * 2.0;
    zStep = pow(n, -1) * 2.0; // xStep and zStep are the same, but could be different

    for (int j = 0; j <= n; ++j) {
        x = -1.0 + j * xStep;
        z = -1.0 + i * zStep;

        glTexCoord2d(1, 0); glVertex3d(x, 0, z);
        // Replace this comment with calculation to work out next z value
        glTexCoord2d(1, 0); glVertex3d(x, 0, z + 0.25f);
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

        glTexCoord2d(0, 0); glVertex3d(x1, y1, z1);
        glTexCoord2d(0, 0); glVertex3d(x2, y2, z2);
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

        double z = 0; // x * tan(frogger.rotation);

        glVertex3d(x + frogger.launch_location.x, y, z + frogger.launch_location.z);
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

        double z = 0; // x * tan(frogger.rotation);

        Vector3d start = {frogger.launch_location.x + x , y, z};

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

void build_grid_extras(bool normals) {
    if(!normals) return;

    double x, z, xStep, zStep;
    double n = 8;
    xStep = pow(n, -1) * 2.0;
    zStep = pow(n, -1) * 2.0;

    for(int i = 0; i < n; ++i) {
        for(int j = 0; j <= n; ++j) {
            x = -1.0 + j * xStep;
            z = -1.0 + i * zStep;

            Vector3d start = {x, 0, z};
            Vector3d normal = {0, x, z};
            build_vector(start, normal, 0.1, yellow);
        }
    }
}

void build_obstacles() {
    for (int i = 0; i < N_LOGS; i++) {
        double position = logs[i] - 1.0;
        double offset = i * 0.3;

        glPushMatrix();
        glTranslated(position, 0.055f, 0.2 + offset);
        glRotated(45.0 - i * 25.0, 0, 1.0, 0.0);
        glCallList(the_log);
        glPopMatrix();
    }
    if(mode.textures) glPopAttrib();
    glPopAttrib();

    for (int i = 0; i < N_CARS; i++) {
        double position = cars[i] - 1.0;
        double offset = i * 0.2;

        glPushMatrix();
        glTranslated(position, 0.055f, -0.8 + offset);
        glCallList(the_car);
        glPopMatrix();
    }
    if(mode.textures) glPopAttrib();
    glPopAttrib();
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
