cmake_minimum_required(VERSION 3.6)
project(a2)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -pedantic -Wno-deprecated-declarations")

set(SOURCE_FILES frogger.c vectors.c frog.h colours.h settings.h sphere.h invariants.h camera.h SOIL.h texture.h texture.c)

add_executable(frogger ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})

target_link_libraries(frogger ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} m)
