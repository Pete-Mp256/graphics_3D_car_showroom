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

bool Canvas::savePPM(const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out << "P6\n" << w_ << ' ' << h_ << "\n255\n";
    for (const Color& c : pixels_) {
        const char bytes[3] = {static_cast<char>(c.r), static_cast<char>(c.g), static_cast<char>(c.b)};
        out.write(bytes, 3);
    }
    return static_cast<bool>(out);
}

} // namespace cg
