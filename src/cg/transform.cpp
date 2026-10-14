#include "cg/transform.hpp"
#include <cmath>

namespace cg {

Mat3 Mat3::translate(double tx, double ty) {
    Mat3 t;
    t.m[0][2] = tx;
    t.m[1][2] = ty;
    return t;
}

Mat3 Mat3::scale(double sx, double sy) {
    Mat3 t;
    t.m[0][0] = sx;
    t.m[1][1] = sy;
    return t;
}

Mat3 Mat3::rotate(double a) {
    Mat3 t;
    double c = std::cos(a), s = std::sin(a);
    t.m[0][0] = c; t.m[0][1] = -s;
    t.m[1][0] = s; t.m[1][1] = c;
    return t;
}

// Composite transform: move the pivot to the origin, transform, move back.
Mat3 Mat3::rotateAbout(double a, const Vec2& p) {
    return translate(p.x, p.y) * rotate(a) * translate(-p.x, -p.y);
}

Mat3 Mat3::scaleAbout(double sx, double sy, const Vec2& p) {
    return translate(p.x, p.y) * scale(sx, sy) * translate(-p.x, -p.y);
}

Mat3 Mat3::operator*(const Mat3& o) const {
    Mat3 r;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            r.m[i][j] = 0;
            for (int k = 0; k < 3; ++k) r.m[i][j] += m[i][k] * o.m[k][j];
        }
    return r;
}

Vec2 Mat3::apply(const Vec2& p) const {
    double x = m[0][0] * p.x + m[0][1] * p.y + m[0][2];
    double y = m[1][0] * p.x + m[1][1] * p.y + m[1][2];
    double w = m[2][0] * p.x + m[2][1] * p.y + m[2][2];
    return {x / w, y / w};
}

} // namespace cg
