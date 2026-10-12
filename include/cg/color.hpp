// color.hpp - RGB colour, HSV conversion and simple shading helpers.
#pragma once
#include <cstdint>

namespace cg {

struct Color {
    std::uint8_t r = 0, g = 0, b = 0;
    bool operator==(const Color& o) const { return r == o.r && g == o.g && b == o.b; }
    bool operator!=(const Color& o) const { return !(*this == o); }
};

Color rgb(int r, int g, int b);                  // clamps to 0..255
Color fromHSV(double hDeg, double s, double v);  // h in degrees, s,v in 0..1
void toHSV(const Color& c, double& hDeg, double& s, double& v);
Color mix(const Color& a, const Color& b, double t); // linear blend, t in 0..1
Color shade(const Color& c, double factor);          // >1 lighter, <1 darker

} // namespace cg
