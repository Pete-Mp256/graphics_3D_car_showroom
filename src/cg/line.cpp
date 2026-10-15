#include "cg/line.hpp"
#include <algorithm>
#include <cmath>
#include "cg/fill.hpp"

namespace cg {

// Digital Differential Analyser: step one pixel along the major axis and add
// the floating-point increment along the other.
void drawLineDDA(Canvas& c, Vec2 a, Vec2 b, Color col) {
    double dx = b.x - a.x, dy = b.y - a.y;
    int steps = static_cast<int>(std::ceil(std::max(std::fabs(dx), std::fabs(dy))));
    if (steps == 0) {
        c.setPixel(static_cast<int>(std::lround(a.x)), static_cast<int>(std::lround(a.y)), col);
        return;
    }
    double xInc = dx / steps, yInc = dy / steps, x = a.x, y = a.y;
    for (int i = 0; i <= steps; ++i) {
        c.setPixel(static_cast<int>(std::lround(x)), static_cast<int>(std::lround(y)), col);
        x += xInc;
        y += yInc;
    }
}

static int r(double v) { return static_cast<int>(std::lround(v)); }

void drawLine(Canvas& c, Vec2 a, Vec2 b, Color col) {
    bresenham(r(a.x), r(a.y), r(b.x), r(b.y), [&](int x, int y) { c.setPixel(x, y, col); });
}

void drawLineAlpha(Canvas& c, Vec2 a, Vec2 b, Color col, double alpha) {
    bresenham(r(a.x), r(a.y), r(b.x), r(b.y), [&](int x, int y) { c.blendPixel(x, y, col, alpha); });
}

void drawDashedLine(Canvas& c, Vec2 a, Vec2 b, Color col, int dash, int gap) {
    int i = 0;
    bresenham(r(a.x), r(a.y), r(b.x), r(b.y), [&](int x, int y) {
        if (i++ % (dash + gap) < dash) c.setPixel(x, y, col);
    });
}

void drawThickLine(Canvas& c, Vec2 a, Vec2 b, double thickness, Color col) {
    Vec2 d = b - a;
    double len = d.length();
    if (len == 0) return;
    Vec2 n = Vec2{-d.y, d.x} * (thickness / 2.0 / len); // perpendicular offset
    fillPolygon(c, Polygon({a + n, b + n, b - n, a - n}), col);
}

std::optional<Vec2> intersectLines(Vec2 a1, Vec2 a2, Vec2 b1, Vec2 b2) {
    Vec2 da = a2 - a1, db = b2 - b1;
    double denom = cross(da, db);
    if (std::fabs(denom) < 1e-12) return std::nullopt;
    double t = cross(b1 - a1, db) / denom;
    return a1 + da * t;
}

double distanceToSegment(Vec2 p, Vec2 a, Vec2 b) {
    Vec2 ab = b - a;
    double len2 = dot(ab, ab);
    double t = len2 == 0 ? 0 : std::clamp(dot(p - a, ab) / len2, 0.0, 1.0);
    return (p - (a + ab * t)).length();
}

} // namespace cg
