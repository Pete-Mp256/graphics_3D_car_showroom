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

} // namespace cg
