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

} // namespace cg
