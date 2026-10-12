#include "cg/curve.hpp"
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

void drawPolyline(Canvas& c, const std::vector<Vec2>& pts, Color col, bool closed) {
    for (std::size_t i = 0; i + 1 < pts.size(); ++i) drawLine(c, pts[i], pts[i + 1], col);
    if (closed && pts.size() > 2) drawLine(c, pts.back(), pts.front(), col);
}

} // namespace cg
