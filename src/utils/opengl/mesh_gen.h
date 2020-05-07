#ifndef __MESH_GEN__
#define __MESH_GEN__

#include <stdio.h>

#include "mesh.h"
#include "../types.h"

template<uint N, uint M>
Mesh gen_plane() {
    static const float L = 1.0;

    const uint num_verts = (N+1)*(M+1);
    const uint num_tris = N*M*2;

    float* verts = new float[num_verts * 3];
    uint* indices = new uint[num_tris * 3];
    float* uvs = new float[num_verts * 2];

    // Generate vertices + UVs
    uint j = 0;
    for (uint i = 0; i < num_verts; i++) {
        uint ii = i % (N+1);
        uint jj = i / (N+1);

        float u = ii / double(N);
        float v = jj / double(M);

        float x = L * (u - 0.5);
        float z = L * (v - 0.5);

        if (ii < N && jj < M) {
            indices[j++] = ii + (N+1)*jj;
            indices[j++] = (ii+1) + (N+1)*jj;
            indices[j++] = (ii+1) + (N+1)*(jj+1);

            indices[j++] = (ii+1) + (N+1)*(jj+1);
            indices[j++] = ii + (N+1)*(jj+1);
            indices[j++] = ii + (N+1)*jj;
        }

        uvs[2*i + 0] = u;
        uvs[2*i + 1] = v;

        verts[3*i + 0] = x;
        verts[3*i + 1] = 0;
        verts[3*i + 2] = z;
    }

    Mesh m(num_verts*3, num_tris*3, verts, indices, uvs, NULL);

    delete [] verts;
    delete [] indices;
    delete [] uvs;

    return m;
}


#endif