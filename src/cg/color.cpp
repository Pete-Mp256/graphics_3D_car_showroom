#include "cg/color.hpp"
#include <algorithm>
#include <cmath>

namespace cg {

static std::uint8_t clampByte(double v) {
    return static_cast<std::uint8_t>(std::clamp(std::lround(v), 0L, 255L));
}

Color rgb(int r, int g, int b) { return {clampByte(r), clampByte(g), clampByte(b)}; }

Color fromHSV(double h, double s, double v) {
    h = std::fmod(h, 360.0);
    if (h < 0) h += 360.0;
    double c = v * s;                                   // chroma
    double x = c * (1 - std::fabs(std::fmod(h / 60.0, 2.0) - 1));
    double m = v - c;
    double r = 0, g = 0, b = 0;
    if (h < 60)       { r = c; g = x; }
    else if (h < 120) { r = x; g = c; }
    else if (h < 180) { g = c; b = x; }
    else if (h < 240) { g = x; b = c; }
    else if (h < 300) { r = x; b = c; }
    else              { r = c; b = x; }
    return {clampByte((r + m) * 255), clampByte((g + m) * 255), clampByte((b + m) * 255)};
}

void toHSV(const Color& col, double& h, double& s, double& v) {
    double r = col.r / 255.0, g = col.g / 255.0, b = col.b / 255.0;
    double mx = std::max({r, g, b}), mn = std::min({r, g, b}), d = mx - mn;
    v = mx;
    s = mx == 0 ? 0 : d / mx;
    if (d == 0)       h = 0;
    else if (mx == r) h = 60 * std::fmod((g - b) / d, 6.0);
    else if (mx == g) h = 60 * ((b - r) / d + 2);
    else              h = 60 * ((r - g) / d + 4);
    if (h < 0) h += 360;
}

} // namespace cg
