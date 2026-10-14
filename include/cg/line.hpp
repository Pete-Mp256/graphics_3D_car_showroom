// line.hpp - line rasterisation (DDA, Bresenham) and line operations.
#pragma once
#include <cstdlib>
#include <optional>
#include "cg/canvas.hpp"
#include "cg/vec2.hpp"

namespace cg {

void drawLineDDA(Canvas& c, Vec2 a, Vec2 b, Color col);

} // namespace cg
