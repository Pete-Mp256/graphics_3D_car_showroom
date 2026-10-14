#include "cg/canvas.hpp"
#include <algorithm>
#include <fstream>

namespace cg {

Canvas::Canvas(int width, int height, Color background)
    : w_(width), h_(height), pixels_(static_cast<std::size_t>(width) * height, background) {}

void Canvas::clear(Color c) { std::fill(pixels_.begin(), pixels_.end(), c); }

void Canvas::setPixel(int x, int y, Color c) {
    if (inBounds(x, y)) pixels_[static_cast<std::size_t>(y) * w_ + x] = c;
}

void Canvas::blendPixel(int x, int y, Color c, double alpha) {
    if (!inBounds(x, y)) return;
    alpha = std::clamp(alpha, 0.0, 1.0);
    Color& dst = pixels_[static_cast<std::size_t>(y) * w_ + x];
    dst = mix(dst, c, alpha); // out = dst*(1-a) + src*a
}

Color Canvas::getPixel(int x, int y) const {
    return inBounds(x, y) ? pixels_[static_cast<std::size_t>(y) * w_ + x] : Color{};
}

} // namespace cg
