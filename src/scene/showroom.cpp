// showroom.cpp - builds the showroom frame from primitives:
//   lines (floor tiles), polygons + clipping (spotlight beams, wheel arches),
//   scan-line / boundary fills, circles & ellipses (platform, wheels),
//   Bezier curves (car body) and splines (wall trim, camera path).
#include "scene/showroom.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

#include "cg/circle.hpp"
#include "cg/clip.hpp"
#include "cg/curve.hpp"
#include "cg/fill.hpp"
#include "cg/line.hpp"
#include "cg/polygon.hpp"
#include "cg/transform.hpp"

using namespace cg;

namespace scene {
namespace {

const Vec2 PLATFORM_CENTRE{480, 440}; // the car stands on the display platform

int ri(double v) { return static_cast<int>(std::lround(v)); }

Shader verticalGradient(Color top, Color bottom, double y0, double y1) {
    return [=](int, int y) { return mix(top, bottom, std::clamp((y - y0) / (y1 - y0), 0.0, 1.0)); };
}

// The car side profile, in car space: x forward (-200..200), y up, ground at y = 0.
const std::vector<Vec2> BODY = {
    {195, 30},  {205, 40},   {206, 60},   {198, 70},    // front bumper
    {170, 82},  {120, 88},   {70, 92},                  // bonnet
    {40, 120},  {20, 135},   {-20, 138},                // windscreen
    {-70, 140}, {-110, 130}, {-140, 100},               // roof and rear window
    {-165, 96}, {-190, 92},  {-200, 80},                // boot
    {-208, 60}, {-206, 40},  {-198, 30},                // rear bumper
    {-100, 26}, {100, 26},   {195, 30},                 // sill
};
const std::vector<Vec2> WINDOW = {
    {62, 92},  {38, 116},  {22, 128},   {-18, 130},
    {-60, 132}, {-95, 124}, {-122, 100},
    {-30, 97},  {30, 94},   {62, 92},
};
constexpr double WHEEL_X = 125, WHEEL_R = 38;

void drawWheel(Canvas& c, const Mat3& toScreen, double scale, double wheelX, double spin) {
    Vec2 centre = toScreen.apply({wheelX, WHEEL_R});
    int cx = ri(centre.x), cy = ri(centre.y), r = ri(WHEEL_R * scale);
    const Color tyre{22, 22, 25}, rimEdge{200, 202, 210}, rimFill{140, 143, 152};

    fillCircle(c, cx, cy, r, tyre);
    int rimR = ri(r * 0.62);
    drawCircle(c, cx, cy, rimR, rimEdge);
    boundaryFill(c, cx, cy, rimFill, rimEdge); // seed fill inside the midpoint-circle outline
    for (int k = 0; k < 5; ++k) {
        Mat3 spoke = Mat3::rotateAbout(spin + k * 2 * PI / 5, centre);
        drawThickLine(c, spoke.apply({centre.x, centre.y - 4}), spoke.apply({centre.x, centre.y - rimR + 2}), 4, {90, 92, 100});
    }
    fillCircle(c, cx, cy, ri(r * 0.16), {60, 62, 68});
}

void drawCar(Canvas& c, Color paint, double spin) {
    const double s = 1.15;
    const Vec2 ground{PLATFORM_CENTRE.x, PLATFORM_CENTRE.y + 14};
    // Car space -> screen: scale (flipping y because screen y points down), then translate.
    const Mat3 toScreen = Mat3::translate(ground.x, ground.y) * Mat3::scale(s, -s);

    fillEllipse(c, ri(ground.x), ri(ground.y), ri(215 * s), 16, Color{0, 0, 0}, 0.45); // contact shadow

    Polygon body = Polygon(sampleCubicPath(BODY, 16)).transformed(toScreen);
    Rect bb = body.bounds();
    fillPolygon(c, body, [&](int, int y) { // simple top-lit shading with a specular band
        double t = (y - bb.ymin) / (bb.ymax - bb.ymin);
        double f = 1.30 - 0.75 * t;
        if (t > 0.38 && t < 0.44) f += 0.35;
        return shade(paint, f);
    });
    drawPolygon(c, body, shade(paint, 0.45));

    // Wheel arches: dark disc clipped (Sutherland-Hodgman) so it only shows above the sill.
    double sillY = toScreen.apply({0, 30}).y;
    for (double wx : {WHEEL_X, -WHEEL_X}) {
        Polygon arch = ellipsePolygon(toScreen.apply({wx, WHEEL_R}), 45 * s, 45 * s, 48);
        fillPolygon(c, clipToRect(arch, {0, 0, double(c.width()), sillY}), Color{18, 18, 20});
    }

    Polygon glass = Polygon(sampleCubicPath(WINDOW, 16)).transformed(toScreen);
    Rect gb = glass.bounds();
    fillPolygon(c, glass, verticalGradient({110, 135, 160}, {28, 34, 46}, gb.ymin, gb.ymax));
    // Reflection streak: a slanted quad clipped to the (convex) window shape.
    Polygon streak({{gb.xmin + 150, gb.ymin - 5}, {gb.xmin + 185, gb.ymin - 5},
                    {gb.xmin + 115, gb.ymax + 5}, {gb.xmin + 80, gb.ymax + 5}});
    if (glass.isConvex()) fillPolygon(c, clipSutherlandHodgman(streak, glass), Color{255, 255, 255}, 0.18);
    drawThickLine(c, toScreen.apply({-30, 131}), toScreen.apply({-26, 94}), 6, shade(paint, 0.8)); // B-pillar

    // Door seams as cubic Bezier curves, handles as short thick lines.
    for (const auto& seam : {std::vector<Vec2>{{58, 92}, {66, 70}, {64, 45}, {55, 30}},
                             std::vector<Vec2>{{-28, 95}, {-24, 70}, {-26, 45}, {-30, 30}},
                             std::vector<Vec2>{{-118, 100}, {-110, 75}, {-108, 50}, {-104, 34}}}) {
        std::vector<Vec2> pts;
        for (const Vec2& p : sampleBezier(seam, 20)) pts.push_back(toScreen.apply(p));
        drawPolyline(c, pts, shade(paint, 0.4));
    }
    drawThickLine(c, toScreen.apply({30, 82}), toScreen.apply({14, 82}), 3, shade(paint, 0.5));
    drawThickLine(c, toScreen.apply({-56, 82}), toScreen.apply({-72, 82}), 3, shade(paint, 0.5));

    Vec2 head = toScreen.apply({190, 62});
    fillEllipse(c, ri(head.x), ri(head.y), ri(10 * s), ri(6 * s), Color{255, 248, 220});
    fillPolygon(c, Polygon({{-205, 66}, {-196, 66}, {-192, 80}, {-200, 82}}).transformed(toScreen), Color{200, 20, 25});

    for (double wx : {WHEEL_X, -WHEEL_X}) drawWheel(c, toScreen, s, wx, spin);
}

} // namespace

void renderShowroom(Canvas& c, const ShowroomOptions& opt) {
    double theta = deg2rad(opt.platformDeg);
    // Layers are drawn back to front: room, spotlights and platform go before the car,
    // the colour picker and minimap after it.
    drawCar(c, opt.paint, theta * 3); // wheels turn three times per platform turn
}

} // namespace scene
