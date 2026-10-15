// circle.hpp - circles and ellipses: midpoint rasterisation and parametric form.
#pragma once
#include "cg/canvas.hpp"
#include "cg/fill.hpp"
#include "cg/polygon.hpp"

namespace cg {

void drawCircle(Canvas& c, int xc, int yc, int r, Color col);  // midpoint circle, 8-way symmetry
void drawEllipse(Canvas& c, int xc, int yc, int rx, int ry, Color col); // midpoint ellipse, 2 regions

} // namespace cg
