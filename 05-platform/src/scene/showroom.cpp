// showroom.cpp - builds the showroom frame from primitives:
//   lines (floor tiles), polygons + clipping (spotlight beams, wheel arches),
//   scan-line / boundary fills, circles & ellipses (platform, wheels),
//   Bezier curves (car body) and splines (wall trim, camera path).
#include "scene/showroom.hpp"

#include <algorithm>
#include <cmath>
#include <utility>
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

constexpr double CEILING_Y = 48, HORIZON_Y = 300;
const Vec2 VANISHING_POINT{480, 210};
const Vec2 PLATFORM_CENTRE{480, 440}; // the car stands on the display platform
constexpr double PLATFORM_RX = 320, PLATFORM_RY = 62, PLATFORM_DEPTH = 18;

int ri(double v) { return static_cast<int>(std::lround(v)); }

Shader verticalGradient(Color top, Color bottom, double y0, double y1) {
    return [=](int, int y) { return mix(top, bottom, std::clamp((y - y0) / (y1 - y0), 0.0, 1.0)); };
}

void drawRoom(Canvas& c) {
    const Rect ceiling{0, 0, double(c.width()), CEILING_Y};
    const Rect wall{0, CEILING_Y, double(c.width()), HORIZON_Y};
    const Rect floor{0, HORIZON_Y, double(c.width() - 1), double(c.height() - 1)};

    fillPolygon(c, Polygon::rectangle(ceiling), Color{28, 30, 36});
    fillPolygon(c, Polygon::rectangle(wall), verticalGradient({62, 66, 78}, {104, 108, 120}, CEILING_Y, HORIZON_Y));
    fillPolygon(c, Polygon::rectangle(floor), verticalGradient({70, 72, 78}, {38, 39, 44}, HORIZON_Y, c.height()));

    // Floor tiles in one-point perspective: rays from the vanishing point, clipped to
    // the floor with Cohen-Sutherland, plus horizontal lines spaced closer near the horizon.
    for (int k = -12; k <= 12; ++k) {
        Vec2 a = VANISHING_POINT, b{480 + k * 170.0, 700};
        if (clipLineCohenSutherland(a, b, floor)) drawLineAlpha(c, a, b, {20, 20, 24}, 0.45);
    }
    for (int k = 1; k <= 8; ++k) {
        double t = k / 8.0, y = HORIZON_Y + (c.height() - HORIZON_Y) * t * t;
        drawLineAlpha(c, {0, y}, {double(c.width()), y}, {20, 20, 24}, 0.45);
    }
    drawLine(c, {0, HORIZON_Y}, {double(c.width()), HORIZON_Y}, {30, 31, 36});

    // Brand trim along the back wall: a closed-off uniform cubic B-spline wave.
    std::vector<Vec2> ctrl;
    for (int i = -1; i <= 13; ++i) ctrl.push_back({i * 80.0, 150.0 + ((i % 2) ? -22.0 : 22.0)});
    std::vector<Vec2> wave = sampleBSpline(ctrl, 12, false);
    for (std::size_t i = 0; i + 1 < wave.size(); ++i) drawThickLine(c, wave[i], wave[i + 1], 3.0, {150, 156, 170});
}

void drawSpotlights(Canvas& c) {
    const Rect room{0, CEILING_Y, double(c.width()), double(c.height())};
    const Color warm{255, 244, 214};
    for (double lx : {200.0, 480.0, 760.0}) {
        double target = lx + (PLATFORM_CENTRE.x - lx) * 0.55; // aim beams towards the car
        // Beams reach past the bottom of the image; Sutherland-Hodgman trims them to the room.
        for (auto [spread, alpha] : {std::pair{170.0, 0.07}, std::pair{95.0, 0.09}}) {
            Polygon beam({{lx - 10, CEILING_Y + 6}, {lx + 10, CEILING_Y + 6},
                          {target + spread, 620}, {target - spread, 620}});
            fillPolygon(c, clipToRect(beam, room), warm, alpha);
        }
        fillEllipse(c, ri(target), ri(PLATFORM_CENTRE.y + 30), 140, 26, warm, 0.10); // light pool
        fillPolygon(c, Polygon({{lx - 16, CEILING_Y - 10}, {lx + 16, CEILING_Y - 10},
                                {lx + 11, CEILING_Y + 6}, {lx - 11, CEILING_Y + 6}}), Color{55, 57, 64});
        fillEllipse(c, ri(lx), ri(CEILING_Y + 6), 10, 3, warm);
    }
}

void drawPlatform(Canvas& c, double theta) {
    int cx = ri(PLATFORM_CENTRE.x), cy = ri(PLATFORM_CENTRE.y);
    int rx = ri(PLATFORM_RX), ry = ri(PLATFORM_RY), depth = ri(PLATFORM_DEPTH);
    const Color side{34, 35, 40}, rim{150, 154, 165};

    // A short cylinder: bottom ellipse + side band + top ellipse.
    fillEllipse(c, cx, cy + depth, rx, ry, side);
    fillPolygon(c, Polygon::rectangle({double(cx - rx), double(cy), double(cx + rx + 1), double(cy + depth)}), side);
    fillEllipse(c, cx, cy, rx, ry, [&](int x, int y) {
        double dx = (x - cx) / double(rx), dy = (y - cy) / double(ry);
        return shade(Color{92, 95, 104}, 1.25 - 0.45 * std::sqrt(dx * dx + dy * dy)); // brushed metal
    });
    drawEllipse(c, cx, cy, rx, ry, rim);

    // Radial grooves rotate with the platform, so the animation is visible.
    for (int k = 0; k < 12; ++k) {
        Vec2 p = ellipsePoint(PLATFORM_CENTRE, PLATFORM_RX - 8, PLATFORM_RY - 3, theta + k * PI / 6);
        drawLineAlpha(c, PLATFORM_CENTRE, p, {60, 62, 70}, 0.6);
    }
    // Rim lights on the front face (sin > 0 means the near side of the ellipse).
    for (int k = 0; k < 18; ++k) {
        double a = theta + k * 2 * PI / 18;
        if (std::sin(a) <= 0.05) continue;
        Vec2 p = ellipsePoint({PLATFORM_CENTRE.x, PLATFORM_CENTRE.y + PLATFORM_DEPTH / 2}, PLATFORM_RX, PLATFORM_RY, a);
        fillCircle(c, ri(p.x), ri(p.y), 3, {120, 210, 255});
    }
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
    drawRoom(c);
    drawSpotlights(c);
    drawPlatform(c, theta);
    drawCar(c, opt.paint, theta * 3); // wheels turn three times per platform turn
}

} // namespace scene
