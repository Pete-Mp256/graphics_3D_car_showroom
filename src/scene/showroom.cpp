// showroom.cpp - builds the showroom frame from primitives:
//   lines (floor tiles), polygons + clipping (spotlight beams, wheel arches),
//   scan-line / boundary fills, circles & ellipses (platform, wheels),
//   Bezier curves (car body) and splines (wall trim, camera path).
#include "scene/showroom.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

#include "cg/circle.hpp"
#include "cg/curve.hpp"
#include "cg/fill.hpp"
#include "cg/polygon.hpp"
#include "cg/transform.hpp"

using namespace cg;

namespace scene {
namespace {

const Vec2 PLATFORM_CENTRE{480, 440}; // the car stands on the display platform

int ri(double v) { return static_cast<int>(std::lround(v)); }

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

void drawCar(Canvas& c, Color paint) {
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
}

} // namespace

void renderShowroom(Canvas& c, const ShowroomOptions& opt) {
    // Layers are drawn back to front: room, spotlights and platform go before the car,
    // the colour picker and minimap after it.
    drawCar(c, opt.paint);
}

} // namespace scene
