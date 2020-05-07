#ifndef __MODEL_H__
#define __MODEL_H__

#include <leon/transform.h>
#include "mesh.h"

template<typename M = Mesh>
class Model {
public:
    Model(M mesh_): mesh(mesh_) {}
    ~Model() { mesh.remove(); }

    void render() { mesh.render(); }

    M& get_mesh() { return mesh; }
    Transform& get_transform() { return transform; }
private:
    M mesh;
    Transform transform;
};

#endif