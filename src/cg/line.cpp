#include "cg/line.hpp"
#include <algorithm>
#include <cmath>

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

} // namespace cg
