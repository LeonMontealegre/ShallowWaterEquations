#ifndef __SHALLOW_WATER_MODEL_H__
#define __SHALLOW_WATER_MODEL_H__

#include <GLFW/glfw3.h>
#include <leon/vector.h>
#include <leon/matrix.h>

#include "utils/types.h"
#include "utils/opengl/model.h"
#include "utils/opengl/displacement_mesh.h"
#include "utils/opengl/mesh_gen.h"
#include "utils/opengl/shader.h"




template<uint N, uint M, uint L = 1>
class ShallowWaterModel {
public:
    ShallowWaterModel(double dt_, double hM, double h0, double damp_, Shader* shaders_[L+1]);
    ~ShallowWaterModel();

    void update();
    void render(const Matrix4f& viewMat);

    Transform& get_transform();

private:
    double dt, dx, dy;
    double g = 1;
    double damp;
    uint t = 0;

    Matrix<N,M> u[L], prev_u[L];
    Matrix<N,M> v[L], prev_v[L];
    Matrix<N,M> h[L], prev_h[L];
    Matrix<N,M> h_B;

    float densities[L+1];

    Model<DisplacementMesh>* surfaces[L];
    Model<DisplacementMesh> ground;

    Shader* shaders[L+1];

    void step();
    void recalculate_normals(Model<DisplacementMesh>& m, const Matrix<N,M>& h);
    double calc_total_energy() const;
    Matrix<N,M> get_pressure(uint i);
};

template<uint N, uint M, uint L>
ShallowWaterModel<N,M,L>::ShallowWaterModel(double dt_, double hM, double h0, double damp_, Shader* shaders_[L+1]):
        ground(DisplacementMesh(gen_plane<N-1,M-1>(), GL_STATIC_DRAW)),
        dt(dt_), dx(1.0 / N), dy(1.0 / M), damp(damp_) {
    // Store shaders
    for (uint i = 0; i < L+1; i++)
        shaders[i] = shaders_[i];

    // Generate surfaces
    for (uint i = 0; i < L; i++) {
        surfaces[i] = new Model<DisplacementMesh>(DisplacementMesh(gen_plane<N-1,M-1>(), GL_DYNAMIC_DRAW));
        surfaces[i]->get_transform().scale(5, 1, 5);
    }
    ground.get_transform().scale(5, 1, 5);

    densities[0] = 0;
    for (uint i = 1; i < L+1; i++)
        densities[i] = 1 + (i-1)/3.0;


    // Set ground height
    for (uint x = 0; x < N; x++) {
        double xx = (double)x / (N-1) - 0.1;
        double xx2 = (double)x / (N-1) - 0.9;
        for (uint y = 0; y < M; y++) {
            double yy = (double)y / (M-1) - 0.1;
            double yy2 = (double)y / (M-1) - 0.9;
            // h_B[x][y] = 0.04/sqrt(2*PI*0.1*0.1) * exp(-(xx*xx + yy*yy)/(2*0.1*0.1));
            // h_B[x][y] += 0.04/sqrt(2*PI*0.1*0.1) * exp(-(xx2*xx2 + yy2*yy2)/(2*0.1*0.1));

            ground.get_mesh().set_displacement(x*N + y, Vecf(0, h_B[x][y], 0));
        }
    }
    recalculate_normals(ground, h_B);
    ground.get_mesh().static_displace();

    // Initial height
    double sigma = 0.05;
    for (uint x = 0; x < N; x++) {
        double xx = (double)x / (N-1) - 5.0/7;
        double xx2 = (double)x / (N-1) - 0.123;
        for (uint y = 0; y < M; y++) {
            double yy = (double)y / (M-1) - 3.0/4;
            double yy2 = (double)y / (M-1) - 0.5643;

            h[0][x][y] = h0 + hM * exp(-(xx*xx + yy*yy)/(2*sigma*sigma));
            h[0][x][y] += hM * exp(-(xx2*xx2 + yy2*yy2)/(2*sigma*sigma));
            prev_h[0][x][y] = h[0][x][y];

            surfaces[0]->get_mesh().set_displacement(x*N + y, Vecf(0, h[0][x][y], 0));
        }
    }

    // Set sub-surface heights to be flat
    for (uint i = 1; i < L; i++) {
        for (uint x = 0; x < N; x++) {
            for (uint y = 0; y < M; y++) {
                prev_h[i][x][y] = h[i][x][y] = (L - i) * h0 / L;
                surfaces[i]->get_mesh().set_displacement(x*N + y, Vecf(0, h[i][x][y], 0));
            }
        }
    }

    for (uint i = 0; i < L; i++) {
        recalculate_normals(*surfaces[i], h[i]);
        surfaces[i]->get_mesh().displace();
    }
}

template<uint N, uint M, uint L>
ShallowWaterModel<N,M,L>::~ShallowWaterModel() {
    for (uint i = 0; i < L; i++)
        delete surfaces[i];
}


template<uint N, uint M, typename T>
T delta_x(const Matrix<N,M,T>& m, int x, int y) {
    return m[x+1][y] - m[x-1][y];
}
template<uint N, uint M, typename T>
T delta_y(const Matrix<N,M,T>& m, int x, int y) {
    return m[x][y+1] - m[x][y-1];
}
template<uint N, uint M, uint L>
Matrix<N,M> ShallowWaterModel<N,M,L>::get_pressure(uint i) {
    Matrix<N,M> p;
    for (uint j = 0; j <= i; j++)
        p += g * (densities[j+1] - densities[j]) * h[j];
    return p;
}

template<uint N, uint M, uint L>
void ShallowWaterModel<N,M,L>::step() {
    for (uint i = 0; i < L; i++) {
        Matrix<N,M> next_u, next_v, next_h;

        Matrix<N,M> eta = h[i] - (i+1 < L ? h[i+1] : 0) - h_B;

        Matrix<N,M> p = get_pressure(i);

        for (uint x = 1; x < N-1; x++) {
            for (uint y = 1; y < M-1; y++) {
                next_u[x][y] = u[i][x][y] - dt * (u[i][x][y]*delta_x(u[i],x,y)/dx  +  v[i][x][y]*delta_y(u[i],x,y)/dy  +  1/densities[i+1]*delta_x(p,x,y)/dx  +  damp*u[i][x][y]);
                next_v[x][y] = v[i][x][y] - dt * (u[i][x][y]*delta_x(v[i],x,y)/dx  +  v[i][x][y]*delta_y(v[i],x,y)/dy  +  1/densities[i+1]*delta_y(p,x,y)/dy  +  damp*v[i][x][y]);

                next_h[x][y] = h[i][x][y] - dt * (u[i][x][y]*delta_x(eta,x,y)/dx   +  v[i][x][y]*delta_y(eta,x,y)/dy   +  eta[x][y]*(delta_x(u[i],x,y)/dx + delta_y(v[i],x,y)/dy));
            }
        }
        for (uint x = 0; x < N; x++) {
            next_h[x][0] = next_h[x][1];
            next_h[x][M-1] = next_h[x][M-2];
        }
        for (uint y = 0; y < M; y++) {
            next_h[0][y] = next_h[1][y];
            next_h[N-1][y] = next_h[N-2][y];
        }

        prev_u[i] = u[i];
        prev_v[i] = v[i];
        prev_h[i] = h[i];
        u[i] = next_u;
        v[i] = next_v;
        h[i] = next_h;
    }
}

template<uint N, uint M, uint L>
void ShallowWaterModel<N,M,L>::recalculate_normals(Model<DisplacementMesh>& m, const Matrix<N,M>& h) {
    const double dx_w = 1.0 / (N-1);
    const double dz_w = 1.0 / (M-1);
    uint i = 0;
    for (uint x = 0; x < N; x++) {
        for (uint y = 0; y < M; y++) {
            Vec3f n;
            if (x >= 1 && y >= 1 && x < N-1 && y < M-1) {
                const double dy1 = (h[x+1][y] - h[x-1][y]);
                const double dy2 = (h[x][y+1] - h[x][y-1]);

                n = Vecf(-2*dy1*dz_w, 4*dx_w*dz_w, -2*dx_w*dy2).norm();
            }
            else {
                n = Vecf(0, 1, 0);
            }
            m.get_mesh().set_normal(i++, n);
        }
    }
}

template<uint N, uint M, uint L>
void ShallowWaterModel<N,M,L>::update() {
    for (uint i = 0; i < 10; i++)
        step();

    for (uint i = 0; i < L; i++) {
        for (uint x = 0; x < N; x++) {
            for (uint y = 0; y < M; y++)
                surfaces[i]->get_mesh().set_displacement(x*N + y, Vecf(0, h[i][x][y], 0));
        }
        recalculate_normals(*surfaces[i], h[i]);

        surfaces[i]->get_mesh().displace();
    }

    // if (t % 60 == 0) {
    //     printf("Total energy: %.8f\n", calc_total_energy());
    // }

    t++;
}

template<uint N, uint M, uint L>
double ShallowWaterModel<N,M,L>::calc_total_energy() const {
    double E = 0;
    // for (uint x = 0; x < N; x++) {
    //     for (uint y = 0; y < M; y++) {
    //         double PE = 0.5 * g * h[x][y]*h[x][y];
    //         double KE = 0.5 * h[x][y] * Vec(u[x][y], v[x][y]).len2();
    //         E += PE + KE;
    //     }
    // }
    return E;
}

template<uint N, uint M, uint L>
void ShallowWaterModel<N,M,L>::render(const Matrix4f& viewMat) {
    shaders[0]->set_uniform("viewMatrix", viewMat);
    shaders[0]->set_uniform("modelMatrix", *ground.get_transform());
    ground.render();

    for (uint i = 0; i < L; i++) {
        shaders[i+1]->set_uniform("viewMatrix", viewMat);
        shaders[i+1]->set_uniform("modelMatrix", *surfaces[i]->get_transform());
        surfaces[i]->render();
    }
}

#endif