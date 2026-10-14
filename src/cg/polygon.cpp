#include "cg/polygon.hpp"
#include <algorithm>
#include <cmath>

namespace cg {

double Polygon::signedArea() const {
    double s = 0;
    for (std::size_t i = 0, n = pts.size(); i < n; ++i)
        s += cross(pts[i], pts[(i + 1) % n]);
    return s / 2.0;
}

double Polygon::area() const { return std::fabs(signedArea()); }

double Polygon::perimeter() const {
    double p = 0;
    for (std::size_t i = 0, n = pts.size(); i < n; ++i) p += (pts[(i + 1) % n] - pts[i]).length();
    return p;
}

Vec2 Polygon::centroid() const {
    double a = signedArea();
    if (std::fabs(a) < 1e-12) { // degenerate: average of vertices
        Vec2 s;
        for (const Vec2& p : pts) s = s + p;
        return pts.empty() ? s : s * (1.0 / pts.size());
    }
    double cx = 0, cy = 0;
    for (std::size_t i = 0, n = pts.size(); i < n; ++i) {
        const Vec2 &p = pts[i], &q = pts[(i + 1) % n];
        double k = cross(p, q);
        cx += (p.x + q.x) * k;
        cy += (p.y + q.y) * k;
    }
    return {cx / (6 * a), cy / (6 * a)};
}

} // namespace cg
