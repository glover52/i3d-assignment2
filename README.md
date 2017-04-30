Assignment 2: Frogger
=====================

> Submission by Adam Thalhammer (s3544305)

Interactive 3D Graphics and Animation
-------------------------------------

### Checklist
"[ ]" = Not Started
"[-]" = Started
"[X]" = Completed

- [x] Draw a sphere to represent a frog using parametric equations.
- [-] Draw the parabola using parametric equations (but now in three dimensions).
- [-] Control the x and z direction of the frog (ie, rotation around the y axis).
- [X] Control the speed/rotation of the frog’s jump (as per assignment 1).
- [-] Animate the frog using the numerical integration technique from assignment 1, but now in three dimensions. No analytical integration required.
- [X] Use a third person perspective camera, that follows the frog as it moves.
- [X] Draw a simple world (just a grid on the xz plane for now).
- [X] Draw moving cylinders to represent logs, and moving cubes to represent cars.
- [-] Draw normal vectors for all geometry being rendered (only tangents for the parabola however).
- [X] Apply lighting to all geometry.
- [-] Apply texturing to the logs, and optionally cars and ground plane.
- [X] Add keyboard controls for for toggling lighting/texturing/wireframe mode

### Compile and Run

    make
    ./frogger

### Controls
'o': toggle axes
'n': toggle normals
'p': toggle wireframe
'l': toggle lighting
't': toggle textures
'w': increase speed
's': decrease speed
'a': increase angle
'd': decrease angle
'space': jump
'left mouse': rotate camera
'right mouse': zoom camera
'left arrow': rotate frog left
'right arrow': rotate frog right
'+/-': double/halve segment amount
