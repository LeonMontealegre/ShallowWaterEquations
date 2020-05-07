#ifndef __MESH_H__
#define __MESH_H__

#ifndef GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLCOREARB
#endif

#include <GLFW/glfw3.h>
#include "constants.h"
#include "../types.h"

struct Primitive {
    static const Primitive Quad;
    static const Primitive Quad2x2;

    int num_verts;
    int num_indices;
    float* verts;
    uint* indices;
    float* texcoords;

    ~Primitive() {
        delete [] verts;
        delete [] indices;
        delete [] texcoords;
    }
};

const Primitive Primitive::Quad = {
    4*3, 2*3,
    new float[4*3]{-0.5,0,-0.5,  0.5,0,-0.5,  -0.5,0,0.5,  0.5,0,0.5},
    new uint[2*3]{0,1,3, 3,2,0},
    new float[4*2]{0,1, 1,1, 1,0, 0,0}
};

const Primitive Primitive::Quad2x2 = {
    9*3, 8*3,
    new float[9*3]{-0.5,0,-0.5,  0,0,-0.5,  0.5,0,-0.5,  -0.5,0,0,  0,0,0,  0.5,0,0,  -0.5,0,0.5,  0,0,0.5,  0.5,0,0.5},
    new uint[8*3]{0,1,4, 4,3,0,  1,2,5, 5,4,1,  3,4,7, 7,6,3,  4,5,8, 8,7,4},
    new float[9*2]{0,1, 1,1, 1,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0}
};

class Mesh {
public:
    Mesh(int num_verts, int num_indices, const float* verts, const uint* indices, const float* tex_coords, const float* normals);
    Mesh(const Primitive& p);

    void bind() const;
    void unbind() const;
    void draw() const;
    void render() const;
    void render(uint amt) const;
    void remove();

    uint num_verts() const { return vert_count; }
    GLuint operator * () const { return vao; }

private:
    GLuint vao, vbo, ibo, tbo, nbo;
    uint count, vert_count;
};



Mesh::Mesh(int num_verts, int num_indices, const float* verts, const uint* indices, const float* tex_coords, const float* normals) {
    vert_count = num_verts;
    count = num_indices;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, num_verts * sizeof(float), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(VERTEX_ATTRIB);
    glVertexAttribPointer(VERTEX_ATTRIB, 3, GL_FLOAT, false, 0, 0);

    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, (num_verts * 2 / 3) * sizeof(float), tex_coords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(TCOORD_ATTRIB);
    glVertexAttribPointer(TCOORD_ATTRIB, 2, GL_FLOAT, false, 0, 0);

    if (normals != NULL) {
        glGenBuffers(1, &nbo);
        glBindBuffer(GL_ARRAY_BUFFER, nbo);
        glBufferData(GL_ARRAY_BUFFER, num_verts * sizeof(float), normals, GL_STATIC_DRAW);
        glEnableVertexAttribArray(NORMAL_ATTRIB);
        glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, false, 0, 0);
    }

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Mesh::Mesh(const Primitive& p): Mesh(p.num_verts, p.num_indices, p.verts, p.indices, p.texcoords, NULL) {
}


void Mesh::bind() const {
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

void Mesh::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::draw() const {
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
}

void Mesh::render() const {
    bind();
    draw();
}

void Mesh::render(uint amt) const {
    bind();
    glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0, amt);
}

void Mesh::remove() {
    unbind();
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &tbo);
    glDeleteBuffers(1, &nbo);
    glDeleteBuffers(1, &ibo);
}




#endif