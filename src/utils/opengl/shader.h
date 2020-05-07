#ifndef __SHADER_H__
#define __SHADER_H__

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <map>
#include <string>

#include <leon/vector.h>
#include <leon/matrix.h>

#include "constants.h"

class Shader {
public:
    Shader(const std::string& vert_shader, const std::string& frag_shader);
    ~Shader();

    void enable();
    void disable();

    GLuint get_uniform(const char* name);
    void set_uniform(const char* name, int value);
    void set_uniform(const char* name, float x);
    void set_uniform(const char* name, double x) { set_uniform(name, (float)x); }

    void set_uniform(const char* name, float x, float y);
    void set_uniform(const char* name, const Vec2& v) { set_uniform(name, v[0], v[1]); }
    void set_uniform(const char* name, const Vec2f& v) { set_uniform(name, v[0], v[1]); }

    void set_uniform(const char* name, float x, float y, float z);
    void set_uniform(const char* name, const Vec3& v) { set_uniform(name, v[0], v[1], v[2]); }
    void set_uniform(const char* name, const Vec3f& v) { set_uniform(name, v[0], v[1], v[2]); }

    void set_uniform(const char* name, float x, float y, float z, float w);
    void set_uniform(const char* name, const Vec4& v) { set_uniform(name, v[0], v[1], v[2], v[3]); }
    void set_uniform(const char* name, const Vec4f& v) { set_uniform(name, v[0], v[1], v[2], v[3]); }

    void set_uniform(const char* name, const Matrix4f& mat);

private:
    static Shader* current;

    GLuint program;
    bool enabled;
    std::map<std::string, GLuint> uniforms_locations;

    GLuint add_shader(const char* shader, GLenum type);
};

Shader* Shader::current = NULL;

Shader::Shader(const std::string& vert_shader, const std::string& frag_shader) {
    program = glCreateProgram();

    GLuint vertID = add_shader(vert_shader.c_str(), GL_VERTEX_SHADER);
    GLuint fragID = add_shader(frag_shader.c_str(), GL_FRAGMENT_SHADER);

    // Extremely important, binds attributes to correct indices
    // Otherwise it can be entirely 'random' and cause strange errors
    glBindAttribLocation(program, VERTEX_ATTRIB, "inPosition");
    glBindAttribLocation(program, TCOORD_ATTRIB, "inTexCoords");
    glBindAttribLocation(program, NORMAL_ATTRIB, "inNormal");

    glLinkProgram(program);
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        char error_msg[1024];
        glGetProgramInfoLog(program, 1024, NULL, error_msg);
        fprintf(stderr, "ERROR LINKING PROGRAM: %s\n", error_msg);
        exit(EXIT_FAILURE);
    }

    glValidateProgram(program);

    glDeleteShader(vertID);
    glDeleteShader(fragID);

    enabled = false;
}

Shader::~Shader() {
    disable();
    glDeleteProgram(program);
    program = -1;
    uniforms_locations.clear();
}



void Shader::enable() {
    if (current != NULL)
        current->disable();
    current = this;
    glUseProgram(program);
    enabled = true;
}

void Shader::disable() {
    if (current == this) {
        current = NULL;
        glUseProgram(0);
    }
    enabled = false;
}


GLuint Shader::get_uniform(const char* name) {
    std::string s_name(name);
    if (uniforms_locations.count(s_name) > 0)
        return uniforms_locations[s_name];

    GLuint result = glGetUniformLocation(program, name);
    if (result == -1)
        fprintf(stderr, "Could not find uniform variable '%s'!\n", name);
    else
        uniforms_locations[s_name] = result;

    return result;
}

void Shader::set_uniform(const char* name, int value) {
    if (!enabled) enable();
    glUniform1i(get_uniform(name), value);
}

void Shader::set_uniform(const char* name, float x) {
    if (!enabled) enable();
    glUniform1f(get_uniform(name), x);
}

void Shader::set_uniform(const char* name, float x, float y) {
    if (!enabled) enable();
    glUniform2f(get_uniform(name), x, y);
}

void Shader::set_uniform(const char* name, float x, float y, float z) {
    if (!enabled) enable();
    glUniform3f(get_uniform(name), x, y, z);
}

void Shader::set_uniform(const char* name, float x, float y, float z, float w) {
    if (!enabled) enable();
    glUniform4f(get_uniform(name), x, y, z, w);
}

void Shader::set_uniform(const char* name, const Matrix4f& mat) {
    if (!enabled) enable();
    glUniformMatrix4fv(get_uniform(name), 1, false, *mat.flatten());
}


GLuint Shader::add_shader(const char* shader, GLenum type) {
    GLuint id = glCreateShader(type);

    glShaderSource(id, 1, &shader, NULL);

    glCompileShader(id);
    GLint success = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        char error_msg[1024];
        glGetShaderInfoLog(id, 1024, NULL, error_msg);
        fprintf(stderr, "ERROR COMPILING SHADER: %s\n", error_msg);
        glDeleteShader(id);
        exit(EXIT_FAILURE);
    }

    glAttachShader(program, id);
    return id;
}

#endif