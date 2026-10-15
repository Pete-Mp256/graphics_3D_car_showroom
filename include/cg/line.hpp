// line.hpp - line rasterisation (DDA, Bresenham) and line operations.
#pragma once
#include <cstdlib>
#include <optional>
#include "cg/canvas.hpp"
#include "cg/vec2.hpp"

namespace cg {

// Bresenham's integer line algorithm for all octants. Calls plot(x, y) once per pixel,
// from (x0,y0) to (x1,y1) inclusive. Templated so the same core drives solid, dashed
// and blended lines.
template <typename Plot>
void bresenham(int x0, int y0, int x1, int y1, Plot plot) {
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy; // decision variable
    while (true) {
        plot(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void drawLineDDA(Canvas& c, Vec2 a, Vec2 b, Color col);
void drawLine(Canvas& c, Vec2 a, Vec2 b, Color col); // Bresenham on rounded endpoints
void drawLineAlpha(Canvas& c, Vec2 a, Vec2 b, Color col, double alpha);
void drawDashedLine(Canvas& c, Vec2 a, Vec2 b, Color col, int dash, int gap);
void drawThickLine(Canvas& c, Vec2 a, Vec2 b, double thickness, Color col); // as a filled quad

// Intersection of the infinite lines through (a1,a2) and (b1,b2); empty if parallel.
std::optional<Vec2> intersectLines(Vec2 a1, Vec2 a2, Vec2 b1, Vec2 b2);
// Shortest distance from p to the segment ab.
double distanceToSegment(Vec2 p, Vec2 a, Vec2 b);

} // namespace cg
