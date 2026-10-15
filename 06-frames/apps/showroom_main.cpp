// showroom_main.cpp - command-line front end for the Milestone 1 showroom renderer.
//
//   showroom [--color NAME | --hue DEGREES] [--angle DEGREES] [--frames N] [--out PATH]
//
// Writes a PPM image (or N animation frames) of the showroom scene.
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>

#include "cg/canvas.hpp"
#include "scene/palette.hpp"
#include "scene/showroom.hpp"

namespace fs = std::filesystem;

static void usage() {
    std::cout << "Usage: showroom [--color NAME | --hue DEG] [--angle DEG] [--frames N] [--out PATH]\n"
                 "  --color   paint colour: red, blue, green, yellow, silver, black (default red)\n"
                 "  --hue     custom paint hue in degrees (0-360), keeps the default saturation/value\n"
                 "  --angle   platform rotation in degrees (default 0)\n"
                 "  --frames  render N frames of one full platform turn (default 1)\n"
                 "  --out     output file (single frame) or directory (frames); default output/\n";
}

int main(int argc, char** argv) {
    scene::ShowroomOptions opt;
    const auto& ps = scene::paints();
    opt.paint = scene::paintColor(ps[0]);
    int frames = 1;
    std::string out;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto next = [&]() -> std::string {
            if (i + 1 >= argc) { std::cerr << "missing value for " << a << "\n"; std::exit(2); }
            return argv[++i];
        };
        if (a == "--color") {
            std::string name = next();
            bool found = false;
            for (std::size_t k = 0; k < ps.size(); ++k)
                if (name == ps[k].name) { opt.paint = scene::paintColor(ps[k]); opt.selectedSwatch = int(k); found = true; }
            if (!found) { std::cerr << "unknown colour '" << name << "'\n"; usage(); return 2; }
        } else if (a == "--hue") {
            opt.paint = cg::fromHSV(std::atof(next().c_str()), ps[0].saturation, ps[0].value);
            opt.selectedSwatch = -1;
        } else if (a == "--angle") {
            opt.platformDeg = std::atof(next().c_str());
        } else if (a == "--frames") {
            frames = std::max(1, std::atoi(next().c_str()));
        } else if (a == "--out") {
            out = next();
        } else if (a == "-h" || a == "--help") {
            usage();
            return 0;
        } else {
            std::cerr << "unknown option '" << a << "'\n";
            usage();
            return 2;
        }
    }

    cg::Canvas canvas(960, 540);
    if (frames == 1) {
        fs::path path = out.empty() ? fs::path("output/showroom.ppm") : fs::path(out);
        if (path.has_parent_path()) fs::create_directories(path.parent_path());
        scene::renderShowroom(canvas, opt);
        if (!canvas.savePPM(path.string())) { std::cerr << "could not write " << path << "\n"; return 1; }
        std::cout << "wrote " << path.string() << "\n";
        return 0;
    }

    fs::path dir = out.empty() ? fs::path("output/frames") : fs::path(out);
    fs::create_directories(dir);
    double start = opt.platformDeg;
    for (int f = 0; f < frames; ++f) {
        opt.platformDeg = start + 360.0 * f / frames;
        scene::renderShowroom(canvas, opt);
        char name[32];
        std::snprintf(name, sizeof name, "frame_%03d.ppm", f);
        if (!canvas.savePPM((dir / name).string())) { std::cerr << "could not write frame " << f << "\n"; return 1; }
    }
    std::cout << "wrote " << frames << " frames to " << dir.string() << "\n";
    return 0;
}
