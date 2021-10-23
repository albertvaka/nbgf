#pragma once

#include "SDL_gpu.h"

#include "vec.h"
#include "bounds.h"

namespace Window {

	namespace DrawPrimitive {

		// pass thickness = 1 to draw one pixel (virtual, after scaling)
		void Point(float x, float y, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a=255);
		inline void Point(vec v, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a=255) { Point(v.x, v.y, thickness, r, g, b, a); }
		inline void Point(float x, float y, float thickness, const SDL_Color& c) { Point(x, y, thickness, c.r, c.g, c.b, c.a); }
		inline void Point(vec v, float thickness, const SDL_Color & c) { Point(v.x, v.y, thickness, c.r, c.g, c.b, c.a); }

		// pass thickness < 0 to draw a filled shape
		void Rectangle(float x1, float y1, float x2, float y2, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		inline void Rectangle(vec va, vec vb, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			Rectangle(va.x, va.y, vb.x, vb.y, thickness, r, g, b, a);
		}
		inline void Rectangle(const BoxBounds& box, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			Rectangle(box.Left(), box.Top(), box.Right(), box.Bottom(), thickness, r, g, b, a);
		}
		inline void Rectangle(float x1, float y1, float x2, float y2, float thickness, const SDL_Color& c) {
			Rectangle(x1, y1, x2, y2, thickness, c.r, c.g, c.b, c.a);
		}
		inline void Rectangle(vec a, vec b, float thickness, const SDL_Color& c) {
			Rectangle(a.x, a.y, b.x, b.y, thickness, c.r, c.g, c.b, c.a);
		}
		inline void Rectangle(const BoxBounds& box, float thickness, const SDL_Color& c) {
			Rectangle(box.Left(), box.Top(), box.Right(), box.Bottom(), thickness, c.r, c.g, c.b, c.a);
		}

		void Line(float x1, float y1, float x2, float y2, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		inline void Line(vec v1, vec v2, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			Line(v1.x, v1.y, v2.x, v2.y, thickness, r, g, b, a);
		}
		inline void Line(float x1, float y1, float x2, float y2, float thickness, const SDL_Color& c) {
			Line(x1, y1, x2, y2, thickness, c.r, c.g, c.b, c.a);
		}
		inline void Line(vec v1, vec v2, float thickness, const SDL_Color & c) {
			Line(v1.x, v1.y, v2.x, v2.y, thickness, c.r, c.g, c.b, c.a);
		}

		inline void Arrow(vec from, vec to, float thickness, float head_lenght, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			Window::DrawPrimitive::Line(from, to, thickness, r,g,b,a);
			vec toTheSide = (to-from).Perp().Normalized() * head_lenght;
			Window::DrawPrimitive::Line(to, to - toTheSide.RotatedAroundOriginDegs(-45), thickness, r, g, b, a);
			Window::DrawPrimitive::Line(to, to + toTheSide.RotatedAroundOriginDegs(45), thickness, r, g, b, a);
		}
		inline void Arrow(vec from, vec to, float thickness, float head_lenght, const SDL_Color& c) {
			Arrow(from, to, thickness, head_lenght, c.r, c.g, c.b, c.a);
		}

		// pass thickness < 0 to draw a filled shape
		void Circle(float x, float y, float radius, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		inline void Circle(vec v, float radius, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			Circle(v.x, v.y, radius, thickness, r, g, b, a);
		}
		inline void Circle(const CircleBounds& bounds, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			Circle(bounds.pos, bounds.radius, thickness, r, g, b, a);
		}
		inline void Circle(float x, float y, float radius, float thickness, const SDL_Color& c) {
			Circle(x, y, radius, thickness, c.r, c.g, c.b, c.a);
		}
		inline void Circle(vec v, float radius, float thickness, const SDL_Color& c) {
			Circle(v.x, v.y, radius, thickness, c.r, c.g, c.b, c.a);
		}
		inline void Circle(const CircleBounds& bounds, float thickness, const SDL_Color& c) {
			Circle(bounds.pos, bounds.radius, thickness, c.r, c.g, c.b, c.a);
		}

		// pass thickness < 0 to draw a filled shape
		void Arc(float x, float y, float radius, float start_angle, float end_angle, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		inline void Arc(vec v, float radius, float start_angle, float end_angle, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			Arc(v.x, v.y, radius, start_angle, end_angle, thickness, r, g, b, a);
		}
		inline void Arc(float x, float y, float radius, float start_angle, float end_angle, float thickness, const SDL_Color& c) {
			Arc(x, y, radius, start_angle, end_angle, thickness, c.r, c.g, c.b, c.a);
		}
		inline void Arc(vec v, float radius, float start_angle, float end_angle, float thickness, const SDL_Color& c) {
			Arc(v.x, v.y, radius, start_angle, end_angle, thickness, c.r, c.g, c.b, c.a);
		}
	}

}
