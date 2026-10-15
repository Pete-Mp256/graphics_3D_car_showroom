// algorithms_demo.cpp - one labelled-by-position panel per technique, for the checkpoint demo.
//
//   row 1: DDA vs Bresenham | line clipping | polygon ops & transforms | scan-line + flood fill
//   row 2: Sutherland-Hodgman | circles & ellipses | Bezier (de Casteljau) | Catmull-Rom vs B-spline
#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "cg/circle.hpp"
#include "cg/clip.hpp"
#include "cg/curve.hpp"
#include "cg/fill.hpp"
#include "cg/line.hpp"
#include "cg/polygon.hpp"
#include "cg/transform.hpp"

using namespace cg;

namespace {
constexpr int CELL = 240;
const Color BG{24, 26, 32}, GRID{60, 63, 72}, FAINT{90, 94, 106}, WHITE{235, 237, 242};
const Color RED{235, 80, 80}, BLUE{90, 160, 255}, GREEN{110, 210, 130}, YELLOW{250, 205, 80};

Vec2 origin(int col, int row) { return {double(col * CELL), double(row * CELL)}; }

void dot(Canvas& c, Vec2 p, Color col) { fillCircle(c, int(std::lround(p.x)), int(std::lround(p.y)), 3, col); }

void lines(Canvas& c, Vec2 o) { // left half DDA, right half Bresenham, same fan of lines
    Vec2 l = o + Vec2{60, 120}, r = o + Vec2{180, 120};
    for (int k = 0; k < 16; ++k) {
        double a = k * PI / 8;
        Vec2 d{50 * std::cos(a), 50 * std::sin(a)};
        drawLineDDA(c, l, l + d, BLUE);
        drawLine(c, r, r + d, GREEN);
    }
    drawDashedLine(c, o + Vec2{20, 200}, o + Vec2{220, 200}, WHITE, 6, 4);
}

void lineClipping(Canvas& c, Vec2 o) {
    Rect win{o.x + 60, o.y + 60, o.x + 180, o.y + 180};
    drawPolygon(c, Polygon::rectangle(win), WHITE);
    for (int k = 0; k < 12; ++k) {
        Vec2 a = o + Vec2{15.0 + k * 19, 15}, b = o + Vec2{225.0 - ((k * 5) % 12) * 19, 225};
        drawLine(c, a, b, FAINT);
        bool ok = (k % 2) ? clipLineLiangBarsky(a, b, win) : clipLineCohenSutherland(a, b, win);
        if (ok) drawLine(c, a, b, k % 2 ? YELLOW : RED);
    }
}

void polygonOps(Canvas& c, Vec2 o) {
    Polygon hex = Polygon::regular(o + Vec2{120, 120}, 45, 6);
    drawPolygon(c, hex, FAINT);
    Mat3 t = Mat3::translate(0, -10) * Mat3::rotateAbout(PI / 7, hex.centroid()) * Mat3::scaleAbout(1.6, 1.6, hex.centroid());
    drawPolygon(c, hex.transformed(t), BLUE);
    dot(c, hex.centroid(), YELLOW);
    // Point-in-polygon test on a grid of samples.
    for (int y = 30; y < 230; y += 14)
        for (int x = 20; x < 230; x += 14) {
            Vec2 p = o + Vec2{double(x), double(y)};
            c.setPixel(int(p.x), int(p.y), hex.transformed(t).contains(p) ? GREEN : GRID);
        }
}

void filling(Canvas& c, Vec2 o) {
    Polygon star;
    for (int k = 0; k < 10; ++k) {
        double a = -PI / 2 + k * PI / 5, rad = (k % 2) ? 38 : 90;
        star.pts.push_back(o + Vec2{120 + rad * std::cos(a), 115 + rad * std::sin(a)});
    }
    fillPolygon(c, star, BLUE); // concave: the edge table handles it
    drawPolygon(c, star, WHITE);
    drawCircle(c, int(o.x + 120), int(o.y + 205), 22, WHITE);
    floodFill(c, int(o.x + 120), int(o.y + 205), RED); // seed fill inside the circle outline
}

} // namespace

int main(int argc, char** argv) {
    std::string out = argc > 1 ? argv[1] : "output/algorithms.ppm";
    Canvas c(4 * CELL, 2 * CELL, BG);
    lines(c, origin(0, 0));
    lineClipping(c, origin(1, 0));
    polygonOps(c, origin(2, 0));
    filling(c, origin(3, 0));
    for (int k = 1; k < 4; ++k) drawLine(c, {double(k * CELL), 0}, {double(k * CELL), double(2 * CELL)}, GRID);
    drawLine(c, {0, double(CELL)}, {double(4 * CELL), double(CELL)}, GRID);

    std::filesystem::path p(out);
    if (p.has_parent_path()) std::filesystem::create_directories(p.parent_path());
    if (!c.savePPM(out)) { std::cerr << "could not write " << out << "\n"; return 1; }
    std::cout << "wrote " << out << "\n";
    return 0;
}
