// transform.hpp - 2D affine transformations as 3x3 matrices in homogeneous coordinates.
//   [x']   [a b c] [x]
//   [y'] = [d e f] [y]
//   [1 ]   [0 0 1] [1]
#pragma once
#include "cg/vec2.hpp"

namespace cg {

struct Mat3 {
    double m[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    static Mat3 identity() { return Mat3{}; }
    static Mat3 translate(double tx, double ty);
    static Mat3 scale(double sx, double sy);
    static Mat3 rotate(double radians); // counter-clockwise about the origin

    Mat3 operator*(const Mat3& o) const; // this applied after o
    Vec2 apply(const Vec2& p) const;
};

} // namespace cg
