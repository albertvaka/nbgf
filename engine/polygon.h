#pragma once

#include <vector>

#include "vec.h"
#include "triangle.h"

typedef std::vector<vec> Polygon;

namespace Polygons {
    [[nodiscard]] vec TopLeft(const Polygon& vertices);

    [[nodiscard]] bool IsClockwise(const Polygon& vertices);

    [[nodiscard]] bool IsPointInside(const Polygon& vertices, const vec& p);

    [[nodiscard]] bool DoesSegmentIntersect(const Polygon& vertices, const vec& p1, const vec& p2);

    // Supports concave polygons
    [[nodiscard]] std::vector<Triangle> Triangulate(const Polygon& polygon);

    [[nodiscard]] Polygon Simplify(const Polygon& outline, float angleTolerance = 0.01f);

    // Uses Chaikin's corner cutting
    [[nodiscard]] Polygon Smoothen(const Polygon& outline, int numPasses = 2);

    [[nodiscard]] Polygon Grow(const Polygon& outline, float thickness); // can be used with negative thickness too

    inline void DebugDraw(const Polygon& outline) {
        for (int i = 0; i < outline.size() - 1; i++) {
            outline[i].DebugDrawAsSegment(outline[i + 1]);
        }
        outline[0].DebugDrawAsSegment(outline[outline.size() - 1]);
    }

}
