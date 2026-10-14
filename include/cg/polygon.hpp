// polygon.hpp - polygon representation (ordered vertex list) and operations.
#pragma once
#include <vector>
#include "cg/canvas.hpp"
#include "cg/transform.hpp"
#include "cg/vec2.hpp"

namespace cg {

struct Polygon {
    std::vector<Vec2> pts; // vertices in order; the last connects back to the first

    Polygon() = default;
    explicit Polygon(std::vector<Vec2> p) : pts(std::move(p)) {}

    std::size_t size() const { return pts.size(); }
    double signedArea() const; // shoelace formula; > 0 counter-clockwise (y up)
    double area() const;
    double perimeter() const;
    Vec2 centroid() const;
    bool isConvex() const;
    bool contains(const Vec2& p) const; // even-odd ray casting
    Rect bounds() const;
};

} // namespace cg
