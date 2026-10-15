#include "cg/clip.hpp"
#include <algorithm>

namespace cg {

// ---- Cohen-Sutherland ------------------------------------------------------
enum : int { INSIDE = 0, LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8 };

static int outcode(const Vec2& p, const Rect& r) {
    int code = INSIDE;
    if (p.x < r.xmin) code |= LEFT; else if (p.x > r.xmax) code |= RIGHT;
    if (p.y < r.ymin) code |= BOTTOM; else if (p.y > r.ymax) code |= TOP;
    return code;
}

bool clipLineCohenSutherland(Vec2& a, Vec2& b, const Rect& r) {
    int ca = outcode(a, r), cb = outcode(b, r);
    while (true) {
        if (!(ca | cb)) return true;  // both inside: trivially accept
        if (ca & cb) return false;    // both share an outside region: trivially reject
        int out = ca ? ca : cb;       // pick an endpoint that is outside
        Vec2 p;
        if (out & TOP)         p = {a.x + (b.x - a.x) * (r.ymax - a.y) / (b.y - a.y), r.ymax};
        else if (out & BOTTOM) p = {a.x + (b.x - a.x) * (r.ymin - a.y) / (b.y - a.y), r.ymin};
        else if (out & RIGHT)  p = {r.xmax, a.y + (b.y - a.y) * (r.xmax - a.x) / (b.x - a.x)};
        else                   p = {r.xmin, a.y + (b.y - a.y) * (r.xmin - a.x) / (b.x - a.x)};
        if (out == ca) { a = p; ca = outcode(a, r); }
        else           { b = p; cb = outcode(b, r); }
    }
}

// ---- Liang-Barsky ----------------------------------------------------------
// Parametric form P(t) = a + t(b - a); shrink [t0, t1] against each boundary.
bool clipLineLiangBarsky(Vec2& a, Vec2& b, const Rect& r) {
    double dx = b.x - a.x, dy = b.y - a.y, t0 = 0, t1 = 1;
    const double p[4] = {-dx, dx, -dy, dy};
    const double q[4] = {a.x - r.xmin, r.xmax - a.x, a.y - r.ymin, r.ymax - a.y};
    for (int i = 0; i < 4; ++i) {
        if (p[i] == 0) {
            if (q[i] < 0) return false; // parallel and outside
            continue;
        }
        double t = q[i] / p[i];
        if (p[i] < 0) t0 = std::max(t0, t); // entering
        else          t1 = std::min(t1, t); // leaving
        if (t0 > t1) return false;
    }
    Vec2 a0 = a;
    a = {a0.x + t0 * dx, a0.y + t0 * dy};
    b = {a0.x + t1 * dx, a0.y + t1 * dy};
    return true;
}

} // namespace cg
