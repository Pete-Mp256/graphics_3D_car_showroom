// vec2.hpp - 2D points/vectors and axis-aligned rectangles.
#pragma once
#include <cmath>

namespace cg {

struct Vec2 {
    double x = 0.0, y = 0.0;
    Vec2() = default;
    Vec2(double x_, double y_) : x(x_), y(y_) {}

    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(double s) const { return {x * s, y * s}; }
    double length() const { return std::sqrt(x * x + y * y); }
};

inline double dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
// z-component of the 3D cross product; > 0 means b is counter-clockwise from a.
inline double cross(const Vec2& a, const Vec2& b) { return a.x * b.y - a.y * b.x; }
inline Vec2 lerp(const Vec2& a, const Vec2& b, double t) { return a + (b - a) * t; }

struct Rect {
    double xmin, ymin, xmax, ymax;
    bool contains(const Vec2& p) const {
        return p.x >= xmin && p.x <= xmax && p.y >= ymin && p.y <= ymax;
    }
};

constexpr double PI = 3.14159265358979323846;
inline double deg2rad(double d) { return d * PI / 180.0; }

} // namespace cg
