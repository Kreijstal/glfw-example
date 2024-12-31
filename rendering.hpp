#ifndef RENDERING_H
#define RENDERING_H

#include "glad/gles2.h"
#include <GLFW/glfw3.h>

bool initializeRendering();
void renderFrame();
void cleanupRendering();

#endif
