#ifndef __DISPLACEMENT_MESH_H__
#define __DISPLACEMENT_MESH_H__

#include <GLFW/glfw3.h>
#include <leon/vector.h>
#include <leon/matrix.h>

#include "constants.h"
#include "../types.h"
#include "mesh.h"

class DisplacementMesh {
public:
    DisplacementMesh(const Mesh& m, GLenum usage_ = GL_STATIC_DRAW);

    void set_displacement(uint i, const Vec3f& d);
    void set_normal(uint i, const Vec3f& d);

    void displace() const;
    void render() const;
    void remove();

    void static_displace();

    GLuint operator * () const { return *mesh; }

protected:
    GLuint dbo, nbo;

    Mesh mesh;

    float* displacement;
    float* normals;

    GLenum usage;

    void add_attribs();
};

DisplacementMesh::DisplacementMesh(const Mesh& m, GLenum usage_): mesh(m), usage(usage_) {
    displacement = new float[m.num_verts()];
    normals      = new float[m.num_verts()];
    for (uint i = 0; i < m.num_verts(); i++) {
        displacement[i] = 0;
        normals[i]      = ((i+2) % 3 == 0 ? 1 : 0);
    }

    // Only add initial for dynamic draw, otherwise wait for
    //  a given displacement/normals and manual call
    if (usage == GL_DYNAMIC_DRAW)
        add_attribs();
}

void DisplacementMesh::add_attribs() {
    glBindVertexArray(*mesh);

    glGenBuffers(1, &nbo);
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.num_verts() * sizeof(float), normals, usage);
    glEnableVertexAttribArray(NORMAL_ATTRIB);
    glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, false, 0, 0);

    glGenBuffers(1, &dbo);
    glBindBuffer(GL_ARRAY_BUFFER, dbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.num_verts() * sizeof(float), displacement, usage);
    glEnableVertexAttribArray(DISPLACEMENT_ATTRIB);
    glVertexAttribPointer(DISPLACEMENT_ATTRIB, 3, GL_FLOAT, false, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void DisplacementMesh::set_displacement(uint i, const Vec3f& d) {
    displacement[3*i + 0] = d[0];
    displacement[3*i + 1] = d[1];
    displacement[3*i + 2] = d[2];
}

void DisplacementMesh::set_normal(uint i, const Vec3f& d) {
    normals[3*i + 0] = d[0];
    normals[3*i + 1] = d[1];
    normals[3*i + 2] = d[2];
}

void DisplacementMesh::displace() const {
    glBindVertexArray(*mesh);

    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.num_verts() * sizeof(float), normals, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, dbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.num_verts() * sizeof(float), displacement, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void DisplacementMesh::static_displace() {
    add_attribs();
}

void DisplacementMesh::render() const {
    mesh.render();
}

void DisplacementMesh::remove() {
    mesh.remove();
    glDeleteBuffers(1, &dbo);
    delete [] displacement;
    delete [] normals;
}

#endif