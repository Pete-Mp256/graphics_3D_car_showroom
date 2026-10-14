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

} // namespace cg
