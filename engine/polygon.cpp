#include "polygon.h"

// This file was mostly vibe-coded

#include <vector>
#include <cmath>
#include <algorithm>

#include "vec.h"
#include "bounds.h"
#include "debug.h"

namespace Polygons {
    [[nodiscard]] vec TopLeft(const Polygon& vertices) {
		if (vertices.empty()) {
			return vec::Zero;
		}
        vec min = vertices[0];
		for (const auto& vertex : vertices) {
			if (vertex.x < min.x) {
				min.x = vertex.x;
			}
			if (vertex.y < min.y) {
				min.y = vertex.y;
			}
		}
		return min;
    }

    [[nodiscard]] bool IsClockwise(const Polygon& vertices) {
        if (vertices.size() < 3) {
            return false;
        }

        float area = 0.0f;
        for (size_t i = 0; i < vertices.size(); ++i) {
            size_t next = (i + 1) % vertices.size();
            area += (vertices[next].x - vertices[i].x) * (vertices[next].y + vertices[i].y);
        }

        return area < 0.0f;
    }

    [[nodiscard]] bool IsPointInside(const Polygon& vertices, const vec& p) {
        bool inside = false;
        for (size_t i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
            if (((vertices[i].y > p.y) != (vertices[j].y > p.y)) &&
                (p.x < (vertices[j].x - vertices[i].x) * (p.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x)) {
                inside = !inside;
            }
        }
        return inside;
    }

    [[nodiscard]] bool DoesSegmentIntersect(const Polygon& vertices, const vec& p1, const vec& p2) {
        for (size_t i = 0; i < vertices.size(); ++i) {
            size_t next = (i + 1) % vertices.size();
            const vec& q1 = vertices[i];
            const vec& q2 = vertices[next];

            // Skip if the segment shares endpoints with the polygon edge
            if ((p1 == q1 && p2 == q2) || (p1 == q2 && p2 == q1)) {
                continue;
            }

            // Check for intersection
            float d1 = (p2 - p1).Cross(q1 - p1);
            float d2 = (p2 - p1).Cross(q2 - p1);
            float d3 = (q2 - q1).Cross(p1 - q1);
            float d4 = (q2 - q1).Cross(p2 - q1);

            if (d1 * d2 < 0 && d3 * d4 < 0) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] std::vector<Triangle> Triangulate(const Polygon& vertices) {
        std::vector<Triangle> result;

        if (vertices.size() < 3) {
            return result;
        }

        // Create a list of indices representing the polygon vertices
        std::vector<int> indices(vertices.size());
        for (size_t i = 0; i < vertices.size(); ++i) {
            indices[i] = static_cast<int>(i);
        }

        // Function to check if a triangle is an ear
        auto isEar = [&vertices, &indices](int i, int n) {
            int prev = (i - 1 + n) % n;
            int curr = i;
            int next = (i + 1) % n;

            // Check if the triangle is counterclockwise
            vec v1 = vertices[indices[next]] - vertices[indices[curr]];
            vec v2 = vertices[indices[prev]] - vertices[indices[curr]];
            if (v1.Cross(v2) <= 0) {
                return false;
            }

            // Check if any of the triangle edges intersect with the polygon
            if (DoesSegmentIntersect(vertices, vertices[indices[prev]], vertices[indices[curr]]) ||
                DoesSegmentIntersect(vertices, vertices[indices[curr]], vertices[indices[next]]) ||
                DoesSegmentIntersect(vertices, vertices[indices[next]], vertices[indices[prev]])) {
                return false;
            }

            // Check if any other vertex is inside the triangle
            Triangle tri(vertices[indices[prev]], vertices[indices[curr]], vertices[indices[next]]);
            for (int j = 0; j < n; ++j) {
                if (j != prev && j != curr && j != next) {
                    if (tri.IsInside(vertices[indices[j]])) {
                        return false;
                    }
                }
            }

            return true;
        };

        // Main ear-clipping algorithm
        int n = static_cast<int>(indices.size());
        int maxIterations = n * n; // Prevent infinite loops
        int iterations = 0;

        while (n > 3 && iterations < maxIterations) {
            iterations++;
            bool earFound = false;

            // Try to find an ear, starting from a random position to avoid bias
            int start = iterations % n;
            for (int offset = 0; offset < n; ++offset) {
                int i = (start + offset) % n;
                if (isEar(i, n)) {
                    // Add the ear to the result
                    int prev = (i - 1 + n) % n;
                    int curr = i;
                    int next = (i + 1) % n;

                    result.emplace_back(vertices[indices[prev]], vertices[indices[curr]], vertices[indices[next]]);

                    // Remove the middle vertex
                    indices.erase(indices.begin() + i);
                    n--;
                    earFound = true;
                    break;
                }
            }

            // If no ear was found, try to find a valid diagonal to split the polygon
            if (!earFound) {
                bool diagonalFound = false;
                for (int i = 0; i < n && !diagonalFound; ++i) {
                    for (int j = i + 2; j < n; ++j) {
                        // Skip adjacent vertices
                        if (j == (i + 1) % n || i == (j + 1) % n) continue;

                        vec p1 = vertices[indices[i]];
                        vec p2 = vertices[indices[j]];

                        // Check if the diagonal is valid
                        if (!DoesSegmentIntersect(vertices, p1, p2)) {
                            // Check if the diagonal is completely inside the polygon
                            vec midPoint = (p1 + p2) * 0.5f;
                            if (IsPointInside(vertices, midPoint)) {
                                // Split the polygon into two parts
                                Polygon part1, part2;

                                // First part: from i to j
                                for (int k = i; k <= j; ++k) {
                                    part1.push_back(vertices[indices[k]]);
                                }

                                // Second part: from j to i
                                for (int k = j; k < n; ++k) {
                                    part2.push_back(vertices[indices[k]]);
                                }
                                for (int k = 0; k <= i; ++k) {
                                    part2.push_back(vertices[indices[k]]);
                                }

                                // Recursively triangulate both parts
                                auto triangles1 = Triangulate(part1);
                                auto triangles2 = Triangulate(part2);

                                // Combine the results
                                result.insert(result.end(), triangles1.begin(), triangles1.end());
                                result.insert(result.end(), triangles2.begin(), triangles2.end());

                                return result;
                            }
                        }
                    }
                }

                // If we couldn't find a valid diagonal, the polygon is too complex
                Debug::out << "Warning: Could not find valid triangulation for complex polygon";
                return result;
            }
        }

        // Add the last triangle
        if (n == 3) {
            result.emplace_back(vertices[indices[0]], vertices[indices[1]], vertices[indices[2]]);
        }

        return result;
    }

    [[nodiscard]] Polygon Simplify(const Polygon& outline, float angleTolerance) {
        if (outline.size() < 3) return outline;

        std::vector<vec> result;
        result.reserve(outline.size());

        // Start with the first point
        result.push_back(outline[0]);

        // For each point (except first and last), check if it can be removed
        for (size_t i = 1; i < outline.size() - 1; i++) {
            vec prev = outline[i - 1];
            vec curr = outline[i];
            vec next = outline[i + 1];

            // Calculate vectors for the two segments
            vec v1 = curr - prev;
            vec v2 = next - curr;

            // Normalize the vectors
            v1.Normalize();
            v2.Normalize();

            // Calculate the angle between the segments using dot product
            float dot = v1.Dot(v2);

            // If segments are nearly aligned (dot product close to 1 or -1)
            // or if the middle point is very close to the line formed by prev and next
            if (std::abs(dot) > std::cos(angleTolerance)) {
                // Skip this point (don't add it to result)
                continue;
            }

            // Add the point if it forms a significant angle
            result.push_back(curr);
        }

        // Always add the last point
        result.push_back(outline.back());

        // If the first and last points are the same (closed polygon), ensure we close it
        if (outline.front().Distance(outline.back()) < 0.01f) {
            result.back() = result.front();
        }

        Debug::out << "Simplified from " << outline.size() << " to " << result.size() << " vertices";

        return result;
    }

    [[nodiscard]] Polygon Smoothen(const Polygon& outline, int numPasses) {
        if (outline.size() < 3) return outline;

        std::vector<vec> result = outline;

        for (int pass = 0; pass < numPasses; pass++) {
            std::vector<vec> temp;
            
            // For each segment we will add two new points
            temp.reserve(result.size() * 2);
            
            for (size_t i = 0; i < result.size(); i++) {
                vec current = result[i];
                vec next = result[(i + 1) % result.size()];

                // Add point at 1/4 of the segment
                temp.push_back(current * 0.75f + next * 0.25f);

                // Add point at 3/4 of the segment
                temp.push_back(current * 0.25f + next * 0.75f);
            }

            result = temp;
        }

        return result;
    }

    [[nodiscard]] Polygon Grow(const Polygon& outline, float thickness) {
        if (outline.size() < 3) return outline;

        Polygon result;
        result.reserve(outline.size());

        // For each vertex, calculate the offset based on the perpendicular bisector
        for (size_t i = 0; i < outline.size(); ++i) {
            const vec& curr = outline[i];
            const vec& prev = outline[(i + outline.size() - 1) % outline.size()];
            const vec& next = outline[(i + 1) % outline.size()];

            // Get edge vectors
            vec prevEdge = curr - prev;
            vec nextEdge = next - curr;

            // Calculate perpendicular vectors (rotate 90 degrees counterclockwise)
            vec prevNormal = vec(-prevEdge.y, prevEdge.x).Normalized();
            vec nextNormal = vec(-nextEdge.y, nextEdge.x).Normalized();

            // Calculate the angle between the edges
            float dot = prevEdge.Normalized().Dot(nextEdge.Normalized());
            float angle = std::acos(std::clamp(dot, -1.0f, 1.0f));

            // Calculate the offset direction
            vec offsetDir;
            if (std::abs(angle - M_PI) < 0.01f) {
                // Special case: 180 degrees (collinear edges)
                offsetDir = prevNormal;
            } else {
                // Normal case: use the average of the two normals
                offsetDir = (prevNormal + nextNormal).Normalized();

                // Adjust the offset length based on the angle
                float scale = 1.0f / std::max(0.01f, std::sin(angle / 2.0f));

                // Limit the scale for very sharp angles
                scale = std::min(scale, 4.0f);

                offsetDir = offsetDir * scale;
            }

            // Check if we need to invert the offset direction
            if (IsClockwise(outline)) {
                offsetDir = -offsetDir;
            }

            // Add the offset vertex
            result.push_back(curr + offsetDir * thickness);
        }

        // Ensure the polygon is closed if it was originally closed
        if (outline.front().Distance(outline.back()) < 0.01f) {
            result.back() = result.front();
        }

        return result;
    }
}
