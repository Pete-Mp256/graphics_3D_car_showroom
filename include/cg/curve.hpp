// curve.hpp - Bezier curves (de Casteljau) and splines (Catmull-Rom, uniform cubic B-spline).
#pragma once
#include <vector>
#include "cg/canvas.hpp"
#include "cg/vec2.hpp"

namespace cg {

// Bezier of any degree, evaluated with de Casteljau's repeated linear interpolation.
Vec2 bezierPoint(const std::vector<Vec2>& ctrl, double t);
std::vector<Vec2> sampleBezier(const std::vector<Vec2>& ctrl, int segments);

// A path of joined cubic Beziers: p0, c1, c2, p1, c1, c2, p2, ... (3n + 1 points).
std::vector<Vec2> sampleCubicPath(const std::vector<Vec2>& pts, int segmentsPerCurve);

// Catmull-Rom spline: interpolates (passes through) every control point.
Vec2 catmullRom(Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3, double t);
std::vector<Vec2> sampleCatmullRom(const std::vector<Vec2>& pts, int segmentsPerSpan, bool closed);

// Uniform cubic B-spline: approximates the control points, C2-continuous.
Vec2 bsplinePoint(Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3, double t);
std::vector<Vec2> sampleBSpline(const std::vector<Vec2>& pts, int segmentsPerSpan, bool closed);

void drawPolyline(Canvas& c, const std::vector<Vec2>& pts, Color col, bool closed = false);

} // namespace cg
