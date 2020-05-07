#ifndef __INIT_H__
#define __INIT_H__

#include <GLFW/glfw3.h>
#include <stdio.h>
#include "../types.h"

static void error_callback(int error, const char* description) {
	fprintf(stderr, "GL Error: %s\n", description);
}

bool initGLFW(uint v_maj = 3, uint v_min = 2, uint msaa_samples = 1, bool forward_compat = true) {
	glfwSetErrorCallback(error_callback);

    // Initialize GLFW
	if (!glfwInit())
		return false;

	// We will ask it to specifically open an OpenGL 3.2 context
	glfwWindowHint(GLFW_SAMPLES, msaa_samples);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, v_maj);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, v_min);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, forward_compat ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return true;
}

#endif