# Milestone 1 plan

## 1. How each required technique fits the showroom

| Required topic | What we implemented | Why it belongs in a car showroom |
|---|---|---|
| Line drawing and line operations | DDA, Bresenham, dashed/thick lines, intersection, distance | Floor tile grid in perspective, door seams, outlines, spokes |
| Polygon representation and operations | Vertex-list polygons, area, centroid, convexity, point-in-polygon, 3x3 matrix transforms | The car body, windows, panels and UI are polygons; transforms place the car and spin it |
| Polygon clipping and filling | Cohen-Sutherland and Liang-Barsky lines, Sutherland-Hodgman polygons; scan-line (ET/AET), flood and boundary fill | Light beams stay inside the room, arches stop at the sill; every surface is a fill with a lighting shader |
| Circles and ellipses | Midpoint circle, midpoint ellipse, parametric ellipse | A circular platform seen in perspective is an ellipse; wheels and spotlights are circles |
| Curves: Bezier and splines | de Casteljau Bezier, cubic Bezier paths, Catmull-Rom, B-spline | Car bodywork is curved; the camera orbit is a smooth spline path |

Those same pieces carry into 3D: the Bezier profile becomes the cross-section the car mesh is
built from, the Mat3 code becomes Mat4 for the camera, and the HSV paint model drives the shader.

## 2. Suggested GitHub workflow so everyone commits

1. Pete Njagi (group leader) creates the repository and adds the other five as collaborators.
2. The first commit is the skeleton only: `README.md`, `.gitignore`, `Makefile`, `CMakeLists.txt`,
   `docs/PLAN.md` and `include/cg/vec2.hpp`. `canvas.*` and `color.*` arrive in their owners' own commits.
3. Each member creates an issue for their area (list below), works on a branch named after it,
   and opens a pull request that another member reviews before merging.
4. Each member commits their own files from their own account. Several small commits
   ("add midpoint ellipse", "fill ellipse rows once for alpha") make a better history than one large one.
5. Run `make test` before every pull request.

Merge in dependency order so the build works at every step:

1. skeleton
2. `color.*` and `canvas.*`
3. lines, polygons and transformations
4. clipping, filling and circles
5. curves
6. the scene interface and car model (`showroom.hpp`, `drawCar`, `drawWheel`)
7. room, spotlights, platform, colour picker, minimap and command-line interface
8. tests, algorithms demo, final README update

## 3. Issues to open

| Issue | Owner | Files |
|---|---|---|
| Project skeleton, build files, README | Pete Njagi (group leader) | `Makefile`, `CMakeLists.txt`, `README.md`, `.gitignore` |
| Framebuffer and PPM output | Danni Podho | `canvas.*` |
| DDA and Bresenham line drawing | Samuel Macharia | `line.*` |
| Cohen-Sutherland and Liang-Barsky line clipping | Samuel Macharia | `clip.*` (line part) |
| Polygon type and operations | Daniel Obuya | `polygon.*` |
| 2D homogeneous transformations | Daniel Obuya | `transform.*` |
| Scan-line, flood and boundary fill | Daniel Obuya | `fill.*` |
| Sutherland-Hodgman polygon clipping | Daniel Obuya | `clip.*` (polygon part) |
| Midpoint circle and ellipse | Mary Arop | `circle.*` |
| Rotating platform and frame export | Mary Arop | `drawPlatform` in `showroom.cpp`, `--frames` in `showroom_main.cpp` |
| Bezier curves and splines | Pete Njagi | `curve.*` |
| Showroom scene interface | Pete Njagi | `showroom.hpp`, `renderShowroom` in `showroom.cpp` |
| Car model from Bezier paths | Pete Njagi | `drawCar`, `drawWheel` in `showroom.cpp` |
| Room, spotlights and floor | Samuel Macharia | `drawRoom`, `drawSpotlights` in `showroom.cpp` |
| RGB/HSV colour model and paint palette | Dylan Saisi | `color.*`, `palette.hpp` |
| Colour picker and minimap UI | Dylan Saisi | `drawColourPicker`, `drawMinimap` in `showroom.cpp` |
| Command-line interface | Dylan Saisi | `showroom_main.cpp` |
| Algorithm tests and demo image | Danni Podho | `tests/`, `apps/algorithms_demo.cpp` |

## 4. For the checkpoint demo

- Show `docs/images/algorithms.png` panel by panel and explain each algorithm.
- Run `make test` live.
- Render the showroom in two colours and at two platform angles to show switching and animation.
- Point at where each technique lives in the scene (the README table).
