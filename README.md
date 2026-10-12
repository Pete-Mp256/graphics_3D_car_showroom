# Virtual Car Showroom

An interactive automotive showroom built from first principles in C++: a car on a rotating
display platform, lit by showroom spotlights, with an on-screen picker for changing its paint
colour and an orbiting camera.

## Project description

Visiting a physical showroom is limited by location, stock and time. Our application lets car
shoppers, dealerships and enthusiasts explore a vehicle from anywhere, inspecting its design,
proportions and colour. It is also a vehicle for us to apply and explain core computer graphics
techniques in a setting where they genuinely matter.

**Milestone 1** is a pure C++ software renderer: every pixel is produced by our own
implementations of the algorithms covered in class and written to a PPM image. No graphics
library is used. Later milestones move the scene into 3D with OpenGL, reusing this code for the
car profile, transformations and colour model.

## Group members and responsibilities

<!-- TODO: confirm each person's area and add GitHub usernames -->
| Member | Role (from our proposal) | Milestone 1 ownership |
|---|---|---|
| Samuel Macharia | Graphics & algorithm implementation | Lines and line clipping (`line.*`, line part of `clip.*`) |
| Daniel Obuya | Graphics & algorithm implementation | Polygons, transformations, polygon clipping and filling (`polygon.*`, `transform.*`, `fill.*`, polygon part of `clip.*`) |
| Pete Njagi (group leader) | 3D modelling, animation & graphics effects | Repository skeleton and README; curves and the car model (`curve.*`, `showroom.hpp`, car profile in `showroom.cpp`) |
| Mary Arop | 3D modelling, animation & graphics effects | Circles/ellipses, platform and animation frames (`circle.*`, platform in `showroom.cpp`) |
| Dylan Saisi | Interface development & testing | Colour model and UI (`color.*`, `palette.hpp`, picker/minimap, `showroom_main.cpp`) |
| Danni Podho | Interface development & testing | Framebuffer, tests and algorithms demo (`canvas.*`, `tests/`, `apps/algorithms_demo.cpp`) |

## Technologies and tools

- **C++17** (standard library only; `<filesystem>` for output folders)
- **g++ / clang++ / MSVC**, built with **Make** or **CMake**
- **PPM (P6)** image output, viewable in GIMP, IrfanView, Photoshop, or VS Code with a PPM extension
- **Git and GitHub** for version control, issues and code review
- Planned for later milestones: **OpenGL** (with GLFW/GLAD and GLM) for the real-time 3D showroom

## Graphics techniques (where each one goes)

Each row is an issue in [docs/PLAN.md](docs/PLAN.md); this table becomes the record of what is
implemented as the pull requests are merged.

| Technique | Where in the code | How the showroom uses it |
|---|---|---|
| Line drawing: DDA and Bresenham (all octants) | `src/cg/line.cpp`, `include/cg/line.hpp` | Floor tile grid, platform grooves, door outlines, polygon edges |
| Line operations: dashed and thick lines, intersection, point-to-segment distance | `src/cg/line.cpp` | Dashed camera path, thick wheel spokes, B-pillar and wall trim |
| Line clipping: Cohen-Sutherland and Liang-Barsky | `src/cg/clip.cpp` | Perspective floor lines clipped to the floor; camera path clipped to the minimap |
| Polygon representation: vertex lists, area (shoelace), centroid, convexity, point-in-polygon | `src/cg/polygon.cpp` | Car body, windows, light fixtures, UI panels |
| 2D transformations: 3x3 homogeneous matrices (translate, rotate, scale, composite about a pivot) | `src/cg/transform.cpp` | Car model space to screen space, wheel spokes, rotating car in the minimap |
| Polygon clipping: Sutherland-Hodgman | `src/cg/clip.cpp` | Spotlight beams clipped to the room, wheel arches clipped to the sill, window reflection clipped to the glass |
| Polygon filling: scan-line with edge table and active edge table, flood fill, boundary fill | `src/cg/fill.cpp` | Every filled surface; gradients and lighting through per-pixel shaders; wheel rims use boundary fill |
| Circles and ellipses: midpoint circle, midpoint ellipse (two regions), parametric form | `src/cg/circle.cpp` | Rotating platform, wheels, light pools, rim lights, colour swatches |
| Bezier curves: de Casteljau for any degree, joined cubic paths | `src/cg/curve.cpp` | The car body and window outline, door seams, rounded UI corners |
| Splines: Catmull-Rom and uniform cubic B-spline | `src/cg/curve.cpp` | Camera orbit path (Catmull-Rom), wall trim (B-spline) |
| Colour: RGB/HSV conversion, blending and shading | `src/cg/color.cpp` | Paint switching by hue, translucent light beams, shaded car paint |

## How to compile and run

Requires a C++17 compiler. From the repository root:

```bash
# Option A: Make (Linux, macOS, MinGW/MSYS2 on Windows)
make            # builds build/showroom, build/algorithms_demo, build/test_algorithms
make test       # runs the algorithm tests
make run        # writes output/showroom.ppm
make demo       # writes output/algorithms.ppm

# Option B: CMake (any platform, including Visual Studio)
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

Showroom options:

```bash
./build/showroom --color blue                    # red, blue, green, yellow, silver, black
./build/showroom --hue 280                       # any paint hue (HSV)
./build/showroom --angle 45                      # rotate the platform
./build/showroom --frames 36 --out output/spin   # 36 frames of one full platform turn
```

To turn frames into a video (optional, needs ffmpeg):
`ffmpeg -framerate 24 -i output/spin/frame_%03d.ppm spin.mp4`

## Current progress

- Repository skeleton: build files (Make and CMake), shared `Vec2` type, plan and work split.
- Next: each member opens their issue from [docs/PLAN.md](docs/PLAN.md) and merges it in the
  dependency order listed there. The commands under "How to compile and run" work once the
  apps and tests are in.

## Known limitations and issues

To be filled in as the modules land.

## Repository structure

```
include/cg/        public headers for the graphics library (one per technique)
src/cg/            algorithm implementations
include/scene/     showroom scene interface and paint palette
src/scene/         the showroom scene, built from the library
apps/              executables: showroom renderer and algorithms demo
tests/             algorithm tests
docs/              plan, work split and screenshots
```
