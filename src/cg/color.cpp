#include "cg/color.hpp"
#include <algorithm>
#include <cmath>

namespace cg {

static std::uint8_t clampByte(double v) {
    return static_cast<std::uint8_t>(std::clamp(std::lround(v), 0L, 255L));
}

Color rgb(int r, int g, int b) { return {clampByte(r), clampByte(g), clampByte(b)}; }

} // namespace cg
