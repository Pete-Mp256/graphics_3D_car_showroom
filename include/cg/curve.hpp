// curve.hpp - Bezier curves (de Casteljau) and splines (Catmull-Rom, uniform cubic B-spline).
#pragma once
#include <vector>
#include "cg/canvas.hpp"
#include "cg/vec2.hpp"

namespace cg {

// Bezier of any degree, evaluated with de Casteljau's repeated linear interpolation.
Vec2 bezierPoint(const std::vector<Vec2>& ctrl, double t);
std::vector<Vec2> sampleBezier(const std::vector<Vec2>& ctrl, int segments);

void drawPolyline(Canvas& c, const std::vector<Vec2>& pts, Color col, bool closed = false);

} // namespace cg
