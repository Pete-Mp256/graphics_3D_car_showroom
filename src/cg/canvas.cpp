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

Color Canvas::getPixel(int x, int y) const {
    return inBounds(x, y) ? pixels_[static_cast<std::size_t>(y) * w_ + x] : Color{};
}

} // namespace cg
