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

static int countColor(const Canvas& c, Color col) {
    int n = 0;
    for (int y = 0; y < c.height(); ++y)
        for (int x = 0; x < c.width(); ++x) n += c.getPixel(x, y) == col;
    return n;
}

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

static void testPolygons() {
    Polygon sq = Polygon::rectangle({0, 0, 4, 4});
    CHECK(near(sq.area(), 16));
    CHECK(near(sq.perimeter(), 16));
    CHECK(near(sq.centroid(), {2, 2}));
    CHECK(sq.isConvex());
    CHECK(sq.contains({1, 1}) && !sq.contains({5, 1}));

    Polygon l({{0, 0}, {4, 0}, {4, 1}, {1, 1}, {1, 4}, {0, 4}}); // L shape
    CHECK(!l.isConvex());
    CHECK(near(l.area(), 7));
    CHECK(!l.contains({3, 3}) && l.contains({0.5, 3}));

    Polygon r = sq.transformed(Mat3::rotateAbout(PI / 2, {2, 2}));
    CHECK(near(r.area(), 16) && near(r.centroid(), {2, 2}));
    CHECK(near(Mat3::translate(3, -1).apply({1, 1}), {4, 0}));
    CHECK(near(Mat3::scaleAbout(2, 2, {1, 1}).apply({2, 2}), {3, 3}));
}

static void testPolygonClipping() {
    Polygon a = Polygon::rectangle({0, 0, 10, 10});
    Polygon b = Polygon::rectangle({5, 5, 15, 15});
    CHECK(near(clipSutherlandHodgman(a, b).area(), 25));
    Polygon cw({{5, 5}, {5, 15}, {15, 15}, {15, 5}}); // same window, opposite winding
    CHECK(near(clipSutherlandHodgman(a, cw).area(), 25));
    CHECK(clipToRect(a, {20, 20, 30, 30}).size() == 0);
}

static void testFilling() {
    Color fg{255, 0, 0};
    Canvas c(20, 20);
    fillPolygon(c, Polygon::rectangle({2, 3, 12, 8}), fg);
    CHECK(countColor(c, fg) == 50); // exactly 10 x 5 pixel centres

    Canvas t(20, 20);
    fillPolygon(t, Polygon({{0, 0}, {10, 0}, {0, 10}}), fg);
    fillPolygon(t, Polygon({{10, 0}, {10, 10}, {0, 10}}), Color{0, 0, 255});
    CHECK(countColor(t, fg) + countColor(t, {0, 0, 255}) == 100); // shared edge, no gaps or overlap

    Canvas f(20, 20);
    drawPolygon(f, Polygon::rectangle({2, 2, 10, 10}), {255, 255, 255});
    floodFill(f, 5, 5, fg);
    CHECK(countColor(f, fg) == 49); // 7 x 7 interior
    Canvas g(20, 20);
    drawCircle(g, 10, 10, 6, {255, 255, 255});
    boundaryFill(g, 10, 10, fg, {255, 255, 255});
    CHECK(g.getPixel(10, 10) == fg && g.getPixel(0, 0) != fg);
}

static void testCircles() {
    Color w{255, 255, 255};
    Canvas c(64, 64);
    drawCircle(c, 32, 32, 20, w);
    bool onRing = true;
    for (int y = 0; y < 64; ++y)
        for (int x = 0; x < 64; ++x)
            if (c.getPixel(x, y) == w && std::fabs(std::hypot(x - 32, y - 32) - 20) > 0.75) onRing = false;
    CHECK(onRing);
    CHECK(c.getPixel(52, 32) == w && c.getPixel(32, 12) == w);

    Canvas e(64, 64);
    drawEllipse(e, 32, 32, 25, 10, w);
    CHECK(e.getPixel(57, 32) == w && e.getPixel(32, 22) == w && e.getPixel(32, 32) != w);

    CHECK(near(ellipsePoint({0, 0}, 3, 2, 0), {3, 0}));
    CHECK(near(ellipsePoint({0, 0}, 3, 2, PI / 2), {0, 2}));
    CHECK(std::fabs(ellipsePolygon({0, 0}, 10, 10, 360).area() - PI * 100) < 0.1);
}

int main() {
    testLines();
    testLineClipping();
    testPolygons();
    testPolygonClipping();
    testFilling();
    testCircles();
    std::cout << (checks - failures) << "/" << checks << " checks passed\n";
    return failures ? 1 : 0;
}
