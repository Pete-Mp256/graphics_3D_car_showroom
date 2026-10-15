// fill.hpp - polygon filling: scan-line (edge table + active edge table),
// flood fill and boundary fill.
#pragma once
#include <functional>
#include "cg/canvas.hpp"
#include "cg/polygon.hpp"

namespace cg {

// A shader returns the colour for pixel (x, y); lets fills do gradients and lighting.
using Shader = std::function<Color(int x, int y)>;

// Scan-line fill using the even-odd rule. Pixel centres (x+0.5, y+0.5) are sampled,
// so adjacent polygons sharing an edge never double-paint.
void fillPolygon(Canvas& c, const Polygon& poly, Color col, double alpha = 1.0);
void fillPolygon(Canvas& c, const Polygon& poly, const Shader& shader, double alpha = 1.0);

} // namespace cg
