// test_algorithms.cpp - small self-checking tests for the cg algorithms (no framework needed).
#include <cmath>
#include <iostream>
#include <set>
#include <utility>

#include "cg/circle.hpp"
#include "cg/clip.hpp"
#include "cg/curve.hpp"
#include "cg/fill.hpp"
#include "cg/line.hpp"
#include "cg/polygon.hpp"
#include "cg/transform.hpp"

using namespace cg;

static int failures = 0, checks = 0;
#define CHECK(cond)                                                                   \
    do {                                                                              \
        ++checks;                                                                     \
        if (!(cond)) { ++failures; std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  " #cond "\n"; } \
    } while (0)

static bool near(double a, double b, double eps = 1e-6) { return std::fabs(a - b) < eps; }
static bool near(Vec2 a, Vec2 b, double eps = 1e-6) { return near(a.x, b.x, eps) && near(a.y, b.y, eps); }

static void testLines() {
    std::set<std::pair<int, int>> px;
    bresenham(0, 0, 10, 4, [&](int x, int y) { px.insert({x, y}); });
    CHECK(px.size() == 11);                       // one pixel per column on a shallow line
    CHECK(px.count({0, 0}) && px.count({10, 4})); // includes both end points
    px.clear();
    bresenham(3, 9, 3, 2, [&](int x, int y) { px.insert({x, y}); });
    CHECK(px.size() == 8);                        // vertical, drawn upwards

    auto p = intersectLines({0, 0}, {10, 10}, {0, 10}, {10, 0});
    CHECK(p && near(*p, {5, 5}));
    CHECK(!intersectLines({0, 0}, {1, 1}, {0, 1}, {1, 2}));
    CHECK(near(distanceToSegment({5, 3}, {0, 0}, {10, 0}), 3));
}

static void testLineClipping() {
    Rect r{0, 0, 10, 10};
    Vec2 a{-5, 5}, b{15, 5};
    CHECK(clipLineCohenSutherland(a, b, r) && near(a, {0, 5}) && near(b, {10, 5}));
    a = {-5, -5}; b = {-1, 20};
    CHECK(!clipLineCohenSutherland(a, b, r));
    a = {-5, 5}; b = {15, 5};
    CHECK(clipLineLiangBarsky(a, b, r) && near(a, {0, 5}) && near(b, {10, 5}));
    a = {20, 20}; b = {30, 30};
    CHECK(!clipLineLiangBarsky(a, b, r));
}

int main() {
    testLines();
    testLineClipping();
    std::cout << (checks - failures) << "/" << checks << " checks passed\n";
    return failures ? 1 : 0;
}
