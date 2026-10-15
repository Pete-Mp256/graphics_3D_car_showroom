// clip.hpp - line clipping (Cohen-Sutherland, Liang-Barsky) and
// polygon clipping (Sutherland-Hodgman).
#pragma once
#include "cg/polygon.hpp"
#include "cg/vec2.hpp"

namespace cg {

// Clip segment a-b to the rectangle in place. Returns false if fully outside.
bool clipLineCohenSutherland(Vec2& a, Vec2& b, const Rect& r);
bool clipLineLiangBarsky(Vec2& a, Vec2& b, const Rect& r);

} // namespace cg
