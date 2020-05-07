#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>

#include "types.h"
#include "color.h"
#include "input.h"

class Window {
public:
    Window(const char* title, uint w, uint h, Color bg_col = Color(0), uint v_sync = 1) {
        col = bg_col;

        window = glfwCreateWindow(w, h, title, NULL, NULL);
        if (!window) {
            std::cerr << "Failed to create window!" << std::endl;
            glfwTerminate();
            exit(1);
        }

        glfwMakeContextCurrent(window);

        glfwSwapInterval(v_sync);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_CULL_FACE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Input::init(window);
    }

    template<typename F>
    void loop(const F& step) {
        int frames = 0;
        double last_time = glfwGetTime();
        while (!should_close()) {
            // Measure FPS
            double cur_time = glfwGetTime();
            frames++;
            if ((cur_time - last_time) >= 1.0) { // If last print was more than 1 sec ago => print and reset timer
                std::cout << frames << " FPS (" << 1000.0/frames << " ms/frame)" << std::endl;
                frames = 0;
                last_time += 1.0;
            }

            glClearColor(col.r, col.g, col.b, col.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            step();

            Input::update();

            // Get error
            GLenum err = glGetError();
            if (err != GL_NO_ERROR)
                std::cerr << "GL Error: " << err << std::endl;

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    void set_bg_color(Color c) {
        col = c;
    }

    bool should_close() const {
        return glfwWindowShouldClose(window);
    }

private:
    GLFWwindow* window;
    Color col;
};

#endif