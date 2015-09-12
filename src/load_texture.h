#ifndef LOAD_TEXTURE
#define LOAD_TEXTURE

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // windows before gl
#endif

#include <GL/glew.h> // glew before gl
#include <GL/gl.h>

/// @return allocated GL textureID for texture given by 'filename'
GLuint loadTextureDDS(const char *filename);

#endif
