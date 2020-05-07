#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB

#include <GLFW/glfw3.h>
#include <iostream>
#include <leon/vector.h>
#include <leon/transform.h>
#include <leon/camera.h>
#include <leon/io_utils.h>
#include <leon/print_utils.h>
#include "utils/opengl/init.h"
#include "utils/opengl/mesh.h"
#include "utils/opengl/shader.h"
#include "utils/opengl/model.h"
#include "utils/opengl/obj_loader.h"
#include "utils/opengl/mesh_gen.h"
#include "utils/opengl/displacement_mesh.h"
#include "utils/window.h"
#include "utils/key.h"
#include "utils/input.h"
#include "shallow_water_model.h"

static const uint WIDTH = 1680, HEIGHT = 945;

void update() {
    if (Input::get_key_down(Key::A)) {
        std::cout << "asd" << std::endl;
    }
}

void render() {

}

int main(void) {
    if (!initGLFW())
        return 1;

    double t = 0;

    Window window("Title", WIDTH, HEIGHT);

	std::cout << "----------------------------------------------------" << std::endl;
	std::cout << "         OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "----------------------------------------------------" << std::endl;


    const double h_B = 1;
    const double h_M = 0.4; // max height diff

    Shader lit_displacement_shader(load_file_as_string("res/displacement_lit.vert"), load_file_as_string("res/lit.frag"));
    Shader unlit_displacement_shader(load_file_as_string("res/displacement.vert"), load_file_as_string("res/default.frag"));
    Shader default_shader(load_file_as_string("res/default.vert"), load_file_as_string("res/default.frag"));
    Shader ocean_shader(load_file_as_string("res/ocean.vert"), load_file_as_string("res/ocean.frag"));

    ShallowWaterModel<75,75,3> swm(0.0001, h_M, h_B, 3, (Shader*[]){&unlit_displacement_shader, &ocean_shader, &lit_displacement_shader, &lit_displacement_shader, &lit_displacement_shader, &lit_displacement_shader});

    Vec3f lightPos = Vecf(0, 1, -10) * 5;

    Camera cam(perspective(70.0f, (float)WIDTH/HEIGHT, 0.1f, 1000.0f), Transform(Vecf(0, -h_B, -3)), Vecf(0, -h_B, 0));
    Model<> sun(load_obj("res/sphere.obj"));

    lit_displacement_shader.set_uniform("projMatrix", cam.get_proj_mat());
    lit_displacement_shader.set_uniform("viewMatrix", *cam.get_transform());
    lit_displacement_shader.set_uniform("color", Vec(0.00, 0.28, 0.73, 1.0));
    lit_displacement_shader.set_uniform("lightDirection", Vec(0, 1, -10).norm());
    lit_displacement_shader.set_uniform("ambientLight", 0.3f);

    unlit_displacement_shader.set_uniform("projMatrix", cam.get_proj_mat());
    unlit_displacement_shader.set_uniform("viewMatrix", *cam.get_transform());

    default_shader.set_uniform("projMatrix", cam.get_proj_mat());
    default_shader.set_uniform("viewMatrix", *cam.get_transform());

    ocean_shader.set_uniform("projMatrix", cam.get_proj_mat());
    ocean_shader.set_uniform("viewMatrix", *cam.get_transform());
    ocean_shader.set_uniform("viewPos", cam.get_transform().get_pos());
    ocean_shader.set_uniform("lightDirection", Vec(0, 1, -10).norm());
    ocean_shader.set_uniform("light.ambient", Vec(1.0, 1.0, 1.0));
    ocean_shader.set_uniform("light.diffuse", Vec(1.0, 1.0, 1.0));
    ocean_shader.set_uniform("light.specular", Vec(1.0, 0.9, 0.7));
    ocean_shader.set_uniform("heightMax", h_B+h_M/3);
    ocean_shader.set_uniform("heightMin", h_B-h_M/20);

    sun.get_transform().scale(0.5);
    sun.get_transform().set_pos(lightPos);

    bool wireframe = false;
    bool paused = true;

    window.set_bg_color(Color(0.49, 0.73, 0.91));
    window.loop([&]() -> void {
        Vec2 dv = Input::get_mouse_change();

        if (Input::get_mouse(Input::LEFT_MOUSE_BUTTON)) {
            cam.get_transform().translate(0, h_B, 0);
            cam.get_transform().rotate(dv[1]/100.0, dv[0]/100.0, 0);
            cam.get_transform().translate(0, -h_B, 0);
        }
        else if (Input::get_mouse(Input::MIDDLE_MOUSE_BUTTON)) {
            cam.get_transform().translate(dv[0]/200.0, -dv[1]/200.0, 0);
        }
        else if (Input::get_mouse(Input::RIGHT_MOUSE_BUTTON)) {
            cam.get_transform().translate(0, 0, -dv[1]/100.0);
        }

        if (Input::get_key_down(Key::W)) {
            wireframe = !wireframe;
            if (wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (Input::get_key_down(Key::P))
            paused = !paused;
        if (Input::get_key_down(Key::SPACEBAR))
            swm.update();

        default_shader.set_uniform("viewMatrix", *cam.get_transform());
        default_shader.set_uniform("modelMatrix", *sun.get_transform());
        sun.render();

        ocean_shader.set_uniform("viewPos", cam.get_transform().get_pos());

        if (!paused)
            swm.update();
        swm.render(*cam.get_transform());

        t++;
    });

    glfwTerminate();
    return 0;
}