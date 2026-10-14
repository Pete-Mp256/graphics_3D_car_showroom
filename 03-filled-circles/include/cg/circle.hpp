// circle.hpp - circles and ellipses: midpoint rasterisation and parametric form.
#pragma once
#include "cg/canvas.hpp"
#include "cg/fill.hpp"
#include "cg/polygon.hpp"

namespace cg {

void drawCircle(Canvas& c, int xc, int yc, int r, Color col);  // midpoint circle, 8-way symmetry
void fillCircle(Canvas& c, int xc, int yc, int r, Color col, double alpha = 1.0);
void drawEllipse(Canvas& c, int xc, int yc, int rx, int ry, Color col); // midpoint ellipse, 2 regions
void fillEllipse(Canvas& c, int xc, int yc, int rx, int ry, Color col, double alpha = 1.0);
void fillEllipse(Canvas& c, int xc, int yc, int rx, int ry, const Shader& shader, double alpha = 1.0);

} // namespace cg
