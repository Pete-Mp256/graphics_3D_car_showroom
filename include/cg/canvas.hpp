// canvas.hpp - a framebuffer we draw into pixel by pixel, saved as a PPM image.
// Every algorithm in this project ends in setPixel()/blendPixel(): no graphics library
// does the rasterisation for us.
#pragma once
#include <string>
#include <vector>
#include "cg/color.hpp"

namespace cg {

class Canvas {
public:
    Canvas(int width, int height, Color background = {0, 0, 0});

    int width() const { return w_; }
    int height() const { return h_; }
    bool inBounds(int x, int y) const { return x >= 0 && y >= 0 && x < w_ && y < h_; }

    void clear(Color c);
    void setPixel(int x, int y, Color c);                 // ignored when out of bounds
    Color getPixel(int x, int y) const;

private:
    int w_, h_;
    std::vector<Color> pixels_;
};

} // namespace cg
