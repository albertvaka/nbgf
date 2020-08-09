#pragma once

#include "bounds.h"

#include "debug.h"

#include "SDL_gpu.h"

namespace Window
{
	extern SDL_Window* window;
	extern GPU_Target* target;
	extern bool focus;
	extern SDL_GLContext glcontext;

	constexpr const int GAME_HEIGHT = 21 * 16;
	constexpr const int GAME_WIDTH = GAME_HEIGHT * 16.f / 9;
	constexpr const char* WINDOW_TITLE = "Gaem";
}

namespace Window
{
	int Init();
	void ProcessEvents();

	inline bool HasFocus() { return focus; }
	bool IsMouseInsideWindow();

	inline void Clear(uint8_t r, uint8_t g, uint8_t b) {
		GPU_ClearRGBA(Window::target, r, g, b, 255);
	}

	namespace DrawPrimitive {

		void Pixel(float x, float y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		inline void Pixel(vec v, uint8_t r, uint8_t g, uint8_t b, uint8_t a) { Pixel(v.x, v.y, r, g, b, a); }
		inline void Pixel(float x, float y, const SDL_Color& c) { Pixel(x, y, c.r, c.g, c.b, c.a); }
		inline void Pixel(vec v, uint8_t r, const SDL_Color & c) { Pixel(v.x, v.y, c.r, c.g, c.b, c.a); }

		// pass thickness = -1 to draw a filled shape
		void Rectangle(float x1, float y1, float x2, float y2, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		inline void Rectangle(const Bounds& box, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			Rectangle(box.Left(), box.Top(), box.Right(), box.Bottom(), thickness, r, g, b, a);
		}
		inline void Rectangle(float x1, float y1, float x2, float y2, float thickness, const SDL_Color& c) {
			Rectangle(x1, y1, x2, y2, thickness, c.r, c.g, c.b, c.a);
		}
		inline void Rectangle(const Bounds& box, float thickness, const SDL_Color& c) {
			Rectangle(box.Left(), box.Top(), box.Right(), box.Bottom(), thickness, c.r, c.g, c.b, c.a);
		}

		void Line(float x1, float y1, float x2, float y2, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		inline void Line(const vec& v1, const vec& v2, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			Line(v1.x, v1.y, v2.x, v2.y, thickness, r, g, b, a);
		}
		inline void Line(float x1, float y1, float x2, float y2, float thickness, const SDL_Color& c) {
			Line(x1, y1, x2, y2, thickness, c.r, c.g, c.b, c.a);
		}
		inline void Line(const vec& v1, const vec& v2, float thickness, const SDL_Color & c) {
			Line(v1.x, v1.y, v2.x, v2.y, thickness, c.r, c.g, c.b, c.a);
		}

		void Circle(float x, float y, float radius, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		inline void Circle(const vec& v, float radius, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			Circle(v.x, v.y, radius, thickness, r, g, b, a);
		}
		inline void Circle(const CircleBounds& bounds, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			Circle(bounds.pos, bounds.radius, thickness, r, g, b, a);
		}
		inline void Circle(float x, float y, float radius, float thickness, const SDL_Color& c) {
			Circle(x, y, radius, thickness, c.r, c.g, c.b, c.a);
		}
		inline void Circle(const vec& v, float radius, float thickness, const SDL_Color& c) {
			Circle(v.x, v.y, radius, thickness, c.r, c.g, c.b, c.a);
		}
		inline void Circle(const CircleBounds& bounds, float thickness, const SDL_Color& c) {
			Circle(bounds.pos, bounds.radius, thickness, c.r, c.g, c.b, c.a);
		}

		void Arc(float x, float y, float radius, float start_angle, float end_angle, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		inline void Arc(const vec& v, float radius, float start_angle, float end_angle, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			Arc(v.x, v.y, radius, start_angle, end_angle, thickness, r, g, b, a);
		}
		inline void Arc(float x, float y, float radius, float start_angle, float end_angle, float thickness, const SDL_Color& c) {
			Arc(x, y, radius, start_angle, end_angle, thickness, c.r, c.g, c.b, c.a);
		}
		inline void Arc(const vec& v, float radius, float start_angle, float end_angle, float thickness, const SDL_Color& c) {
			Arc(v.x, v.y, radius, start_angle, end_angle, thickness, c.r, c.g, c.b, c.a);
		}
	}

	class Draw {
		GPU_Image* t;
		vec dest;
		GPU_Rect src = { 0,0,0,0 };
		GPU_Rect* srcp = nullptr;
		float rotation = 0;
		vec scale = vec(1.f, 1.f);
		vec origin = vec(0.f,0.f);
	public:
		constexpr Draw(GPU_Image* t, const vec& pos) : t(t), dest(pos) { }
		constexpr Draw(GPU_Image* t, float x, float y) : t(t), dest(x,y) { }

		constexpr Draw& withRect(float x, float y, float w, float h) {
			return withRect({ x, y, w, h });
		}

		constexpr Draw& withRect(const GPU_Rect& r) {
			src = r;
			srcp = &src;
			return *this;
		}

		Draw& withColor(SDL_Color c) {
			return withColor(c.r, c.g, c.b, c.a);
		}

		Draw& withColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			GPU_SetRGBA(t, r, g, b, a);
			return *this;
		}

		constexpr Draw& withOrigin(float x, float y) {
			origin.x = x;
			origin.y = y;
			return *this;
		}

		constexpr Draw& withOrigin(const vec& o) {
			return withOrigin(o.x, o.y);
		}

		constexpr Draw& withRotation(float degrees) {
			rotation = degrees;
			return *this;
		}

		constexpr Draw& withScale(float x, float y) {
			scale = vec(x, y);
			return *this;
		}

		constexpr Draw& withScale(float s) {
			return withScale(s, s);
		}

		constexpr Draw& withScale(const vec& v) {
			return withScale(v.x, v.y);
		}

		~Draw() {
			// We pass origin as rotation pivot. We could change that to a different variable.
			GPU_BlitTransformX(t, srcp, Window::target, dest.x, dest.y, origin.x, origin.y, rotation, scale.x, scale.y);
			GPU_SetRGBA(t, 255, 255, 255, 255);
		}
	};

	namespace DrawRaw {

		const unsigned short MAX_VERTICES = 60000; //From SDL_GPU's GPU_BLIT_BUFFER_ABSOLUTE_MAX_VERTICES
		const unsigned int MAX_INDICES = (MAX_VERTICES / 4) * 6; //6 indices for each 4 vertices (quads drawn as 2 triangles)
		const unsigned int MAX_COMPONENTS_PER_VERTEX = 8; // Max of GPU_BATCH_XY_ST (4), GPU_BATCH_XY_RGB (5) and GPU_BATCH_XY_ST_RGBA (8) 

		extern unsigned short vertex_count;
		extern unsigned int index_count;
		extern float vertices[MAX_VERTICES * MAX_COMPONENTS_PER_VERTEX];
		extern unsigned short indices[MAX_INDICES];

		inline void FlushTexturedQuads(GPU_Image* t) {
			//Debug::out << "vertices:" << vertex_count << " indices:" << index_count;
			GPU_TriangleBatch(t, Window::target, vertex_count, vertices, index_count, indices, GPU_BATCH_XY_ST);
			vertex_count = 0;
			index_count = 0;
		}

		inline void FlushRGBQuads() {
			//Debug::out << "vertices:" << vertex_count << " indices:" << index_count;
			GPU_TriangleBatch(NULL, Window::target, vertex_count, vertices, index_count, indices, GPU_BATCH_XY_RGB);
			vertex_count = 0;
			index_count = 0;
		}

		inline void FlushColoredTexturedQuads(GPU_Image* t) {
			//Debug::out << "vertices:" << vertex_count << " indices:" << index_count;
			GPU_TriangleBatch(t, Window::target, vertex_count, vertices, index_count, indices, GPU_BATCH_XY_ST_RGBA);
			vertex_count = 0;
			index_count = 0;
		}

		//Colors and texture coords between 0 and 1
		inline void BatchColoredTexturedQuad(GPU_Image* t, float x, float y, float w, float h, const GPU_Rect& tr, float r, float g, float b, float a)
		{
			const int COMPONENTS_PER_VERTEX = 8; // GPU_BATCH_XY_ST_RGBA
			unsigned int i = vertex_count * COMPONENTS_PER_VERTEX;

			//bottom left
			vertices[i++] = x;
			vertices[i++] = y + h;
			vertices[i++] = tr.x;
			vertices[i++] = tr.y + tr.h;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;
			vertices[i++] = a;

			//top left
			vertices[i++] = x;
			vertices[i++] = y;
			vertices[i++] = tr.x;
			vertices[i++] = tr.y;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;
			vertices[i++] = a;

			//top right
			vertices[i++] = x + w;
			vertices[i++] = y;
			vertices[i++] = tr.x + tr.w;
			vertices[i++] = tr.y;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;
			vertices[i++] = a;

			//bottom right
			vertices[i++] = x + w;
			vertices[i++] = y + h;
			vertices[i++] = tr.x + tr.w;
			vertices[i++] = tr.y + tr.h;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;
			vertices[i++] = a;

			indices[index_count++] = vertex_count;
			indices[index_count++] = vertex_count + 1;
			indices[index_count++] = vertex_count + 2;
			indices[index_count++] = vertex_count;
			indices[index_count++] = vertex_count + 2;
			indices[index_count++] = vertex_count + 3;

			vertex_count += 4;

			if (vertex_count + 4 >= MAX_VERTICES) {
				// Flush what we have so we don't go over MAX_VERTICES
				FlushColoredTexturedQuads(t);
			};
		}
		
		//Texture coords between 0 and 1
		inline void BatchTexturedQuad(GPU_Image* t, float x, float y, float w, float h, const GPU_Rect& tr)
		{
			const int COMPONENTS_PER_VERTEX = 4; // GPU_BATCH_XY_ST
			unsigned int i = vertex_count * COMPONENTS_PER_VERTEX;

			//bottom left
			vertices[i++] = x;
			vertices[i++] = y + h;
			vertices[i++] = tr.x;
			vertices[i++] = tr.y + tr.h;

			//top left
			vertices[i++] = x;
			vertices[i++] = y;
			vertices[i++] = tr.x;
			vertices[i++] = tr.y;

			//top right
			vertices[i++] = x + w;
			vertices[i++] = y;
			vertices[i++] = tr.x + tr.w;
			vertices[i++] = tr.y;

			//bottom right
			vertices[i++] = x + w;
			vertices[i++] = y + h;
			vertices[i++] = tr.x + tr.w;
			vertices[i++] = tr.y + tr.h;

			indices[index_count++] = vertex_count;
			indices[index_count++] = vertex_count + 1;
			indices[index_count++] = vertex_count + 2;
			indices[index_count++] = vertex_count;
			indices[index_count++] = vertex_count + 2;
			indices[index_count++] = vertex_count + 3;

			vertex_count += 4;

			if (vertex_count + 4 >= MAX_VERTICES) {
				// Flush what we have so we don't go over MAX_VERTICES
				FlushTexturedQuads(t);
			};
		}

		//Colors between 0 and 1
		inline void BatchRGBQuad(float x, float y, float width, float height, float r, float g, float b)
		{
			const int COMPONENTS_PER_VERTEX = 5; // GPU_BATCH_XY_RGB
			unsigned int i = vertex_count * COMPONENTS_PER_VERTEX;

			//bottom left
			vertices[i++] = x;
			vertices[i++] = y + height;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;

			//top left
			vertices[i++] = x;
			vertices[i++] = y;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;

			//top right
			vertices[i++] = x + width;
			vertices[i++] = y;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;

			//bottom right
			vertices[i++] = x + width;
			vertices[i++] = y + height;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;

			indices[index_count++] = vertex_count;
			indices[index_count++] = vertex_count + 1;
			indices[index_count++] = vertex_count + 2;
			indices[index_count++] = vertex_count;
			indices[index_count++] = vertex_count + 2;
			indices[index_count++] = vertex_count + 3;

			vertex_count += 4;

			if (vertex_count + 4 >= MAX_VERTICES) {
				// Flush what we have so we don't go over MAX_VERTICES
				FlushRGBQuads();
			};
		}

	}


}

inline void RectToTextureCoordinates(const GPU_Image* i, GPU_Rect& tr) {
	tr.x /= i->texture_w;
	tr.y /= i->texture_h;
	tr.w /= i->texture_w;
	tr.h /= i->texture_h;
}

inline std::ostream& operator<<(std::ostream& os, const SDL_Point& rhs)
{
	os << rhs.x << "," << rhs.y;
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const SDL_FPoint& rhs)
{
	os << rhs.x << "," << rhs.y;
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const SDL_Rect& rhs)
{
	os << rhs.x << "," << rhs.y << "," << rhs.w << "," << rhs.h;
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const SDL_FRect& rhs)
{
	os << rhs.x << "," << rhs.y << "," << rhs.w << "," << rhs.h;
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const GPU_Rect& rhs)
{
	os << rhs.x << "," << rhs.y << "," << rhs.w << "," << rhs.h;
	return os;
}
