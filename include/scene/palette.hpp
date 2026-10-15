// palette.hpp - paint colours offered by the on-screen colour picker.
#pragma once
#include <array>
#include <string>
#include "cg/color.hpp"

namespace scene {

struct Paint {
    const char* name;
    double hue, saturation, value; // HSV: the UI changes paint by changing these
};

inline const std::array<Paint, 6>& paints() {
    static const std::array<Paint, 6> p{{
        {"red", 355, 0.85, 0.80},
        {"blue", 215, 0.80, 0.75},
        {"green", 150, 0.70, 0.55},
        {"yellow", 45, 0.90, 0.95},
        {"silver", 210, 0.06, 0.78},
        {"black", 220, 0.25, 0.18},
    }};
    return p;
}

inline cg::Color paintColor(const Paint& p) { return cg::fromHSV(p.hue, p.saturation, p.value); }

} // namespace scene
