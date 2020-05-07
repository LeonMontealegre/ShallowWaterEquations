#ifndef __COLOR_H__
#define __COLOR_H__

#include <leon/vector.h>

class Color: public Vector<4> {
public:
    Color(): Vector<4>() {}
    Color(double v, double a = 1): Vector<4>({v, v, v, a}) {}
    Color(double r, double g, double b, double a = 1): Vector<4>({r, g, b, a}) {}
    Color(const Color& c) {
        (*this) = c;
    }
    const Color& operator = (const Color& c) {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
        return *this;
    }

    double& r = (*this)[0];
    double& g = (*this)[1];
    double& b = (*this)[2];
    double& a = (*this)[3];
};

#endif