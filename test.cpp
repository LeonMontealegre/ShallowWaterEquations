#include <iostream>
#include <cmath>
#include "../../CompPhys/lib/utils.h"
#include "../../CompPhys/lib/constants.h"
#include "../../CompPhys/lib/vector.h"
#include "../../CompPhys/lib/matrix.h"
#include "../../CompPhys/lib/gnuplot.h"

int main() {
    static const uint N = 200;
    static const uint N_T = 3000;

    Vector<N> h_B(0.0);
    Vector<N> h(10.0), prev_h(10.0);

    Vector<N> u(0.0), prev_u(0.0);

    double dt = 0.0001;
    double dx = 1.0/N;
    double K = 0.25 * dx*dx / dt / 2;
    double g = 1;

    // Add floor
    for (uint x = 0; x < N; x++) {
        double xx = (double)x / N;
        h_B[x] += 3*std::sin(xx * 9)*std::sin(xx*9);//std::sqrt(2*PI*0.1*0.1) * std::exp(-(xx - 0.25)*(xx - 0.25) / (2 * 0.1*0.1));
        // h_B[x] = 3*(xx < 0.25 ? 1 : (xx < 0.5 ? -4*xx+2 : 0));
        // h[x] = 10.0 - h_B[x];
        // prev_h[x] = h[x];
    }


    // Adds ics
    GNUPlot test("Test", {"x", "h"});
    test.set_axis_range(0, 0, 1);
    test.set_axis_range(1, 0, 15);
    test.add_data_block("Water Height", PlotStyle("filledcurve", "#0000ff"));
    double a = 0.5;
    double mu = 0.5;
    double sigma = 0.1;
    for (uint x = 0; x < N; x++) {
        double xx = (double)x / N;
        h[x] += a/std::sqrt(2*PI*sigma*sigma) * std::exp(-(xx - mu)*(xx - mu) / (2 * sigma*sigma));
        prev_h[x] += a/std::sqrt(2*PI*sigma*sigma) * std::exp(-(xx - mu)*(xx - mu) / (2 * sigma*sigma));
    }
    for (uint x = 0; x < N; x++)
        test.add_data({(double)x/N, h[x]});
    for (int x = N-1; x >= 0; x--)
        test.add_data({(double)x/N, h_B[x]});

    test.execute("initial");




    GNUPlot plot("2D Shallow Water Equation", {"x", "h"});
    plot.set_animated(2);
    plot.set_axis_range(0, 0, 1);
    plot.set_axis_range(1, 0, 15);

    for (uint t = 0; t < N_T; t++) {
        Vector<N> next_u, next_h, next_h_B;
        // plot.add_data(t, h);

        uint i;
        if (t % 20 == 0)
            i = plot.add_data_block("Water Height", PlotStyle("filledcurve", "#0000ff"));

        // next_h[0] = h[0] = prev_h[0] = 10;
        // next_h[N-1] = h[N-1] = prev_h[N-1] = 10;
        // next_u[0] = u[0] = prev_u[0] = 0;
        // next_u[N-1] = u[N-1] = prev_u[N-1] = 0;
        for (uint x = 1; x < N-1; x++) {
            next_h[x] = prev_h[x] - u[x]*dt/dx * ((h[x+1] - h_B[x+1]) - (h[x-1] - h_B[x-1])) - (h[x] - h_B[x])*dt/dx * (u[x+1] - u[x-1]);
            next_u[x] = prev_u[x] - u[x]*dt/dx * (u[x+1] - u[x-1]) - g*dt/dx * (h[x+1] - h[x-1]);

            // next_h[x] = prev_h[x] - 2*u[x]*dt/dx * (h[x+1] - h[x-1]) - 2*h[x]*dt/dx * (u[x+1] - u[x-1]) + K*dt/(dx*dx) * (prev_h[x+1] + prev_h[x-1] - 2*prev_h[x]);
            // next_u[x] = prev_u[x] - 2*u[x]*dt/dx * (u[x+1] - u[x-1]) - 2*g*dt/dx * (h[x+1] - h[x-1]);
        }

        if (t % 20 == 0) {
            for (uint x = 0; x < N; x++)
                plot.add_data(i, {(double)x/N, h[x]});
            for (int x = N-1; x >= 0; x--)
                plot.add_data(i, {(double)x/N, h_B[x]});
        }
        next_h[0] = next_h[1];
        next_h[N-1] = next_h[N-2];

        prev_u = u;
        prev_h = h;
        u = next_u;
        h = next_h;
    }

    plot.execute("test");





    // du/dt + u*du/dx = -g d/dx h
    // dh/dt + u*dh/dx + h * du/dx = 0

    // dh/dt + d/dx (uh) + d/dy (uh) = 0
    // dh/dt + d/dx (vh) + d/dt (vh) = 0

    // (u_{x,t+1} - u_{x,t}) / dt + u_{x,t} * (u_{x+1,t} - u_{x,t}) / dx = -g * (h_{x+1,t} - h_{x,t}) / dx
    // (h_{x,t+1} - h_{x,t}) / dt + u_{x,t} * (u_{x+1,t} - u_{x,t}) / dx + h * (u_{x+1,t} - u_{x,t}) / dx = 0



    return 0;
}