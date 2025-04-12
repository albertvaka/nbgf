#pragma once

#include "vec.h"
#include <vector>
#include "window_conf.h"

struct Triangle {
	vec a, b, c;

	Triangle(vec a, vec b, vec c) : a(a), b(b), c(c) {}

	[[nodiscard]] vec TopLeft() const {
		vec min = a;
		if (b.x < min.x) {
			min.x = b.x;
		}
		if (b.y < min.y) {
			min.y = b.y;
		}
		if (c.x < min.x) {
			min.x = c.x;
		}
		if (c.y < min.y) {
			min.y = c.y;
		}
		return min;
	}

	[[nodiscard]] vec BottomRight() const {
		vec max = a;
		if (b.x > max.x) {
			max.x = b.x;
		}
		if (b.y > max.y) {
			max.y = b.y;
		}
		if (c.x > max.x) {
			max.x = c.x;
		}
		if (c.y > max.y) {
			max.y = c.y;
		}
		return max;
	}

	[[nodiscard]] bool IsInside(const vec& p) const {
		vec v0 = { c.x - a.x, c.y - a.y };
		vec v1 = { b.x - a.x, b.y - a.y };
		vec v2 = { p.x - a.x, p.y - a.y };

		float dot00 = v0.x * v0.x + v0.y * v0.y;
		float dot01 = v0.x * v1.x + v0.y * v1.y;
		float dot02 = v0.x * v2.x + v0.y * v2.y;
		float dot11 = v1.x * v1.x + v1.y * v1.y;
		float dot12 = v1.x * v2.x + v1.y * v2.y;

		float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);

		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		return (u >= 0) && (v >= 0) && (u + v <= 1);
	}

	[[nodiscard]] float Area() const {
		return 0.5f * std::abs((a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)));
	}

	[[nodiscard]] vec Centroid() const {
		return (a + b + c) * (1.0f / 3.0f);
	}

	void GrowFromOrigin(float scale, vec origin = vec::Zero) {
		a = origin + (a - origin) * scale;
		b = origin + (b - origin) * scale;
		c = origin + (c - origin) * scale;
	}

	void GrowFromCenter(float scale) {
		GrowFromOrigin(scale, Centroid());
	}

	[[nodiscard]] Triangle GrownFromOrigin(float scale, vec origin = vec::Zero) const {
		Triangle ret = *this;
		ret.GrowFromOrigin(scale, origin);
		return ret;
	}

	[[nodiscard]] Triangle GrownFromCenter(float scale) const {
		return GrownFromOrigin(scale, Centroid());
	}

	void DebugDraw(uint8_t r = 255, uint8_t g = 0, uint8_t b = 0) const
#ifdef _DEBUG
		;
#else
	{}
#endif
};


struct RgbaTriangle {
	// Order matters for DrawRgbaTriangles
	vec a;
	float ar, ag, ab, aa;
	vec b;
	float br, bg, bb, ba;
	vec c;
	float cr, cg, cb, ca;
	RgbaTriangle(Triangle t, float r, float g, float b, float a)
		: a(t.a), ar(r), ag(g), ab(b), aa(a),
		b(t.b), br(r), bg(g), bb(b), ba(a),
		c(t.c), cr(r), cg(g), cb(b), ca(a)
	{
	}
	Triangle Triangle() const {
		return ::Triangle(a, b, c);
	}
};

inline std::vector<RgbaTriangle> ToRgbaTriangles(const std::vector<Triangle>& triangles, float r, float g, float b, float a) {
	std::vector<RgbaTriangle> ret;
	for (auto& t : triangles) {
		ret.emplace_back(t, r, g, b, a);
	}
	return ret;
}

struct StTriangle {
	// Order matters for DrawStTriangles
	vec a;
	vec sta;
	vec b;
	vec stb;
	vec c;
	vec stc;
	StTriangle(Triangle t, Triangle tt)
		: a(t.a), sta(tt.a)
		, b(t.b), stb(tt.b)
		, c(t.c), stc(tt.c)
	{
	}
	Triangle Triangle() const {
		return ::Triangle(a, b, c);
	}
};

// if maxTexCoords is > 1, you probably wnat to use GPU_SetWrapMode(tex, GPU_WRAP_REPEAT, GPU_WRAP_REPEAT); on the texture
inline std::vector<StTriangle> ToStTriangles(const std::vector<Triangle>& triangles, vec maxTexCoords = vec(1, 1)) {
	vec min = triangles[0].TopLeft();
	vec max = triangles[0].BottomRight();
	for (auto& triangle : triangles) {
		vec v = triangle.TopLeft();
		if (v.x < min.x) min.x = v.x;
		if (v.y < min.y) min.y = v.y;
		if (v.x > max.x) max.x = v.x;
		if (v.y > max.y) max.y = v.y;
	}
	std::vector<StTriangle> ret;
	for (auto& t : triangles) {
		// Convert triangle coordinates to texture coordinates
		vec ta = Mates::Map(t.a, min, max, vec::Zero, maxTexCoords);
		vec tb = Mates::Map(t.b, min, max, vec::Zero, maxTexCoords);
		vec tc = Mates::Map(t.c, min, max, vec::Zero, maxTexCoords);
		//Debug::out << ta << " " << tb << " " << tc;
		//Debug::out << (ta * texSize) << " " << (tb * texSize) << " " << (tc * texSize);
		ret.emplace_back(t, Triangle(ta, tb, tc));
	}
	return ret;
}

inline void DrawRgbaTriangles(const std::vector<RgbaTriangle>& triangles) {
	GPU_PrimitiveBatch(NULL, Window::currentDrawTarget, GPU_TRIANGLES, 3 * triangles.size(), (float*)triangles.data(), 0, NULL, GPU_BATCH_XY_RGBA);
}

inline void DrawStTriangles(GPU_Image* texture, const std::vector<StTriangle>& triangles) {
	GPU_PrimitiveBatch(texture, Window::currentDrawTarget, GPU_TRIANGLES, 3 * triangles.size(), (float*)triangles.data(), 0, NULL, GPU_BATCH_XY_ST);
}

