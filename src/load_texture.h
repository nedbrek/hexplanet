#ifndef LOAD_TEXTURE
#define LOAD_TEXTURE

#ifdef WIN32
#include <windows.h> // windows before gl
#endif

#include <GL/glew.h> // glew before gl
#include <GL/gl.h>
#include <GL/glu.h>

// creates and returns a GL textureID
GLuint loadTextureDDS(const char *filename);

#endif
