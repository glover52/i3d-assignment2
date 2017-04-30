/**
 * Frog: http://seamless-pixels.blogspot.com.au/2012/08/reptile-frog-snake-alien-skin-texture.html
 * Grass: https://www.textures.com/download/grass0164/50573
 * Rock: https://www.textures.com/download/rocksharp0062/68366
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#if _WIN32
#   include <Windows.h>
#endif

#if __APPLE__
#   include <OpenGL/gl.h>
#else
#   include <GL/gl.h>
#endif


#include "SOIL.h"

GLuint loadTexture(const char *filename);

#endif //TEXTURE_H
