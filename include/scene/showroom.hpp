// showroom.hpp - the Virtual Car Showroom scene, drawn entirely with the cg algorithms.
// This is the 2D front-elevation version for Milestone 1; the 3D/OpenGL version will
// reuse the same car profile curves, transformations and colour model.
#pragma once
#include "cg/canvas.hpp"
#include "cg/color.hpp"

namespace scene {

struct ShowroomOptions {
    cg::Color paint{200, 30, 40}; // current car paint colour
    int selectedSwatch = 0;       // which UI swatch is highlighted (-1 = custom hue)
    double platformDeg = 0.0;     // rotation of the display platform (animation)
};

void renderShowroom(cg::Canvas& canvas, const ShowroomOptions& opt);

} // namespace scene
