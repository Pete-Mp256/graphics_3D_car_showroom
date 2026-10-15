#include "cg/fill.hpp"
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace cg {

namespace {
struct Edge {
    int yEnd;         // first scan line the edge no longer covers
    double x;         // x where the edge crosses the current scan line's pixel centre
    double invSlope;  // dx/dy: how much x moves per scan line
};
} // namespace

void fillPolygon(Canvas& c, const Polygon& poly, const Shader& shader, double alpha) {
    if (poly.size() < 3) return;

    // 1. Edge table: bucket each non-horizontal edge by the first scan line it covers.
    //    A scan line y is sampled at y + 0.5 (pixel centre).
    int yMin = c.height(), yMax = -1;
    std::vector<std::pair<int, Edge>> table;
    for (std::size_t i = 0, n = poly.size(); i < n; ++i) {
        Vec2 p = poly.pts[i], q = poly.pts[(i + 1) % n];
        if (p.y == q.y) continue;             // horizontal edges add no crossings
        if (p.y > q.y) std::swap(p, q);       // p is the upper (smaller y) end
        int yStart = static_cast<int>(std::ceil(p.y - 0.5));
        int yEnd = static_cast<int>(std::ceil(q.y - 0.5));
        if (yStart >= yEnd) continue;
        double inv = (q.x - p.x) / (q.y - p.y);
        table.push_back({yStart, Edge{yEnd, p.x + (yStart + 0.5 - p.y) * inv, inv}});
        yMin = std::min(yMin, yStart);
        yMax = std::max(yMax, yEnd);
    }
    std::sort(table.begin(), table.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    // 2. Walk scan lines, maintaining the active edge table (AET).
    std::vector<Edge> active;
    std::size_t next = 0;
    for (int y = std::max(yMin, 0); y < std::min(yMax, c.height()); ++y) {
        // Edges that started above the canvas still need their x advanced to this row.
        while (next < table.size() && table[next].first <= y) {
            Edge e = table[next].second;
            e.x += (y - table[next].first) * e.invSlope;
            active.push_back(e);
            ++next;
        }
        active.erase(std::remove_if(active.begin(), active.end(), [y](const Edge& e) { return e.yEnd <= y; }),
                     active.end());
        std::sort(active.begin(), active.end(), [](const Edge& a, const Edge& b) { return a.x < b.x; });

        // 3. Fill between pairs of crossings (even-odd rule).
        for (std::size_t i = 0; i + 1 < active.size(); i += 2) {
            int xStart = std::max(0, static_cast<int>(std::ceil(active[i].x - 0.5)));
            int xEnd = std::min(c.width(), static_cast<int>(std::ceil(active[i + 1].x - 0.5)));
            for (int x = xStart; x < xEnd; ++x) {
                if (alpha >= 1.0) c.setPixel(x, y, shader(x, y));
                else c.blendPixel(x, y, shader(x, y), alpha);
            }
        }
        // 4. Incrementally move each active edge to the next scan line.
        for (Edge& e : active) e.x += e.invSlope;
    }
}

void fillPolygon(Canvas& c, const Polygon& poly, Color col, double alpha) {
    fillPolygon(c, poly, [col](int, int) { return col; }, alpha);
}

} // namespace cg
