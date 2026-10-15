#include "cg/circle.hpp"
#include <cmath>
#include <vector>

namespace cg {

// Midpoint circle: walk the second octant from (0, r) and mirror into the other seven.
template <typename Plot8>
static void midpointCircle(int r, Plot8 plot) {
    int x = 0, y = r, d = 1 - r; // decision parameter
    while (x <= y) {
        plot(x, y);
        if (d < 0) d += 2 * x + 3;
        else { d += 2 * (x - y) + 5; --y; }
        ++x;
    }
}

void drawCircle(Canvas& c, int xc, int yc, int r, Color col) {
    midpointCircle(r, [&](int x, int y) {
        c.setPixel(xc + x, yc + y, col); c.setPixel(xc - x, yc + y, col);
        c.setPixel(xc + x, yc - y, col); c.setPixel(xc - x, yc - y, col);
        c.setPixel(xc + y, yc + x, col); c.setPixel(xc - y, yc + x, col);
        c.setPixel(xc + y, yc - x, col); c.setPixel(xc - y, yc - x, col);
    });
}

// Midpoint ellipse in two regions: region 1 where the slope magnitude < 1 (step x),
// region 2 where it is > 1 (step y). plot(x, y) gives one point per quadrant.
template <typename Plot4>
static void midpointEllipse(int rx, int ry, Plot4 plot) {
    double rx2 = double(rx) * rx, ry2 = double(ry) * ry;
    int x = 0, y = ry;
    double d1 = ry2 - rx2 * ry + 0.25 * rx2;
    double dx = 2 * ry2 * x, dy = 2 * rx2 * y;
    while (dx < dy) { // region 1
        plot(x, y);
        ++x; dx += 2 * ry2;
        if (d1 < 0) d1 += dx + ry2;
        else { --y; dy -= 2 * rx2; d1 += dx - dy + ry2; }
    }
    double d2 = ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1.0) * (y - 1.0) - rx2 * ry2;
    while (y >= 0) { // region 2
        plot(x, y);
        --y; dy -= 2 * rx2;
        if (d2 > 0) d2 += rx2 - dy;
        else { ++x; dx += 2 * ry2; d2 += dx - dy + rx2; }
    }
}

void drawEllipse(Canvas& c, int xc, int yc, int rx, int ry, Color col) {
    midpointEllipse(rx, ry, [&](int x, int y) {
        c.setPixel(xc + x, yc + y, col); c.setPixel(xc - x, yc + y, col);
        c.setPixel(xc + x, yc - y, col); c.setPixel(xc - x, yc - y, col);
    });
}

void fillEllipse(Canvas& c, int xc, int yc, int rx, int ry, const Shader& shader, double alpha) {
    // The boundary gives, for each row offset y, the widest x; fill each row exactly once
    // so translucent fills don't double-blend.
    std::vector<int> half(ry + 1, -1);
    midpointEllipse(rx, ry, [&](int x, int y) { if (x > half[y]) half[y] = x; });
    for (int y = 0; y <= ry; ++y) {
        if (half[y] < 0) continue;
        for (int sy : {yc - y, yc + y}) {
            for (int x = xc - half[y]; x <= xc + half[y]; ++x) {
                if (alpha >= 1.0) c.setPixel(x, sy, shader(x, sy));
                else c.blendPixel(x, sy, shader(x, sy), alpha);
            }
            if (y == 0) break; // centre row only once
        }
    }
}

void fillEllipse(Canvas& c, int xc, int yc, int rx, int ry, Color col, double alpha) {
    fillEllipse(c, xc, yc, rx, ry, [col](int, int) { return col; }, alpha);
}

void fillCircle(Canvas& c, int xc, int yc, int r, Color col, double alpha) {
    fillEllipse(c, xc, yc, r, r, col, alpha);
}

Vec2 ellipsePoint(Vec2 centre, double rx, double ry, double t, double rotation) {
    double x = rx * std::cos(t), y = ry * std::sin(t);
    double cr = std::cos(rotation), sr = std::sin(rotation);
    return {centre.x + x * cr - y * sr, centre.y + x * sr + y * cr};
}

Polygon ellipsePolygon(Vec2 centre, double rx, double ry, int segments, double rotation) {
    Polygon p;
    for (int i = 0; i < segments; ++i)
        p.pts.push_back(ellipsePoint(centre, rx, ry, 2 * PI * i / segments, rotation));
    return p;
}

} // namespace cg
