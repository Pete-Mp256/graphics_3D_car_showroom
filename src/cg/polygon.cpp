#include "cg/polygon.hpp"
#include <algorithm>
#include <cmath>
#include "cg/line.hpp"

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

// Convex when every consecutive edge pair turns the same way.
bool Polygon::isConvex() const {
    std::size_t n = pts.size();
    if (n < 3) return false;
    int sign = 0;
    for (std::size_t i = 0; i < n; ++i) {
        double z = cross(pts[(i + 1) % n] - pts[i], pts[(i + 2) % n] - pts[(i + 1) % n]);
        if (std::fabs(z) < 1e-12) continue;
        int s = z > 0 ? 1 : -1;
        if (sign == 0) sign = s;
        else if (s != sign) return false;
    }
    return true;
}

// Cast a ray to the right of p and count edge crossings: odd means inside.
bool Polygon::contains(const Vec2& p) const {
    bool inside = false;
    for (std::size_t i = 0, j = pts.size() - 1; i < pts.size(); j = i++) {
        const Vec2 &a = pts[i], &b = pts[j];
        if ((a.y > p.y) != (b.y > p.y)) {
            double xCross = a.x + (p.y - a.y) * (b.x - a.x) / (b.y - a.y);
            if (p.x < xCross) inside = !inside;
        }
    }
    return inside;
}

Rect Polygon::bounds() const {
    Rect r{1e300, 1e300, -1e300, -1e300};
    for (const Vec2& p : pts) {
        r.xmin = std::min(r.xmin, p.x); r.ymin = std::min(r.ymin, p.y);
        r.xmax = std::max(r.xmax, p.x); r.ymax = std::max(r.ymax, p.y);
    }
    return r;
}

Polygon Polygon::transformed(const Mat3& m) const {
    Polygon out;
    out.pts.reserve(pts.size());
    for (const Vec2& p : pts) out.pts.push_back(m.apply(p));
    return out;
}

Polygon Polygon::rectangle(const Rect& r) {
    return Polygon({{r.xmin, r.ymin}, {r.xmax, r.ymin}, {r.xmax, r.ymax}, {r.xmin, r.ymax}});
}

Polygon Polygon::regular(Vec2 c, double radius, int sides, double start) {
    Polygon p;
    for (int i = 0; i < sides; ++i) {
        double a = start + 2 * PI * i / sides;
        p.pts.push_back({c.x + radius * std::cos(a), c.y + radius * std::sin(a)});
    }
    return p;
}

void drawPolygon(Canvas& c, const Polygon& poly, Color col) {
    for (std::size_t i = 0, n = poly.size(); i < n; ++i) drawLine(c, poly.pts[i], poly.pts[(i + 1) % n], col);
}

} // namespace cg
