#include "cg/curve.hpp"
#include <algorithm>
#include "cg/line.hpp"

namespace cg {

// de Casteljau: repeatedly interpolate neighbouring control points until one remains.
Vec2 bezierPoint(const std::vector<Vec2>& ctrl, double t) {
    std::vector<Vec2> tmp = ctrl;
    for (std::size_t level = tmp.size(); level > 1; --level)
        for (std::size_t i = 0; i + 1 < level; ++i) tmp[i] = lerp(tmp[i], tmp[i + 1], t);
    return tmp.empty() ? Vec2{} : tmp[0];
}

std::vector<Vec2> sampleBezier(const std::vector<Vec2>& ctrl, int segments) {
    std::vector<Vec2> out;
    for (int i = 0; i <= segments; ++i) out.push_back(bezierPoint(ctrl, double(i) / segments));
    return out;
}

std::vector<Vec2> sampleCubicPath(const std::vector<Vec2>& pts, int segs) {
    std::vector<Vec2> out;
    for (std::size_t i = 0; i + 3 < pts.size(); i += 3) {
        std::vector<Vec2> piece = sampleBezier({pts[i], pts[i + 1], pts[i + 2], pts[i + 3]}, segs);
        // Skip the first point of every piece after the first: it equals the previous end.
        out.insert(out.end(), piece.begin() + (out.empty() ? 0 : 1), piece.end());
    }
    return out;
}

Vec2 catmullRom(Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3, double t) {
    double t2 = t * t, t3 = t2 * t;
    return (p1 * 2.0 + (p2 - p0) * t + (p0 * 2.0 - p1 * 5.0 + p2 * 4.0 - p3) * t2 +
            (p1 * 3.0 - p0 - p2 * 3.0 + p3) * t3) * 0.5;
}

Vec2 bsplinePoint(Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3, double t) {
    double t2 = t * t, t3 = t2 * t;
    double b0 = (1 - t) * (1 - t) * (1 - t) / 6.0;
    double b1 = (3 * t3 - 6 * t2 + 4) / 6.0;
    double b2 = (-3 * t3 + 3 * t2 + 3 * t + 1) / 6.0;
    double b3 = t3 / 6.0;
    return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
}

// Shared driver: for span i use points i-1..i+2. Open curves clamp indices at the ends
// (for Catmull-Rom that still passes through the first and last points).
template <typename Eval>
static std::vector<Vec2> sampleSpline(const std::vector<Vec2>& p, int segs, bool closed, Eval eval) {
    std::vector<Vec2> out;
    int n = static_cast<int>(p.size());
    if (n < 2) return p;
    auto at = [&](int i) {
        if (closed) return p[((i % n) + n) % n];
        return p[std::clamp(i, 0, n - 1)];
    };
    int spans = closed ? n : n - 1;
    for (int i = 0; i < spans; ++i)
        for (int s = 0; s < segs; ++s) out.push_back(eval(at(i - 1), at(i), at(i + 1), at(i + 2), double(s) / segs));
    if (!closed) out.push_back(eval(at(n - 2), at(n - 1), at(n), at(n + 1), 0.0)); // final end point
    return out;
}

std::vector<Vec2> sampleCatmullRom(const std::vector<Vec2>& pts, int segs, bool closed) {
    return sampleSpline(pts, segs, closed, catmullRom);
}

std::vector<Vec2> sampleBSpline(const std::vector<Vec2>& pts, int segs, bool closed) {
    return sampleSpline(pts, segs, closed, bsplinePoint);
}

void drawPolyline(Canvas& c, const std::vector<Vec2>& pts, Color col, bool closed) {
    for (std::size_t i = 0; i + 1 < pts.size(); ++i) drawLine(c, pts[i], pts[i + 1], col);
    if (closed && pts.size() > 2) drawLine(c, pts.back(), pts.front(), col);
}

} // namespace cg
