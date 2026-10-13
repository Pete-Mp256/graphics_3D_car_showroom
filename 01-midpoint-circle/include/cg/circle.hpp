// circle.hpp - circles and ellipses: midpoint rasterisation and parametric form.
#pragma once
#include "cg/canvas.hpp"
#include "cg/fill.hpp"
#include "cg/polygon.hpp"

namespace cg {

void drawCircle(Canvas& c, int xc, int yc, int r, Color col);  // midpoint circle, 8-way symmetry

} // namespace cg
