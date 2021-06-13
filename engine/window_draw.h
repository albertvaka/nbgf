#pragma once

#include "SDL_gpu.h"

#include "angles.h"
#include "bounds.h"

namespace Window {

	extern GPU_Target* currentDrawTarget;

	struct DeferredDraw {

		constexpr DeferredDraw(GPU_Image* t, vec pos) : t(t), dest(pos) {
			withRect(0,0,t->w,t->h);
		}
		constexpr DeferredDraw(GPU_Image* t, float x, float y) : t(t), dest(x,y) {
			withRect(0,0,t->w,t->h);
		}
		constexpr DeferredDraw(GPU_Image* t, const BoxBounds& destRect) // sets scale
			: t(t), dest(destRect.TopLeft())
			, scale(destRect.width/float(t->w), destRect.height/float(t->h))
		{
			withRect(0,0,t->w,t->h);
		}

		constexpr DeferredDraw& withRect(float x, float y, float w, float h) {
			return withRect({ x, y, w, h });
		}

		constexpr DeferredDraw& withRect(const GPU_Rect& r) {
			src = r;
			return *this;
		}

		constexpr DeferredDraw& withOrigin(float x, float y) {
			origin.x = x;
			origin.y = y;
			return *this;
		}

		constexpr DeferredDraw& withOrigin(vec o) {
			return withOrigin(o.x, o.y);
		}

		constexpr DeferredDraw& withRotationDegs(float degrees) {
			rotation = degrees;
			return *this;
		}

		constexpr DeferredDraw& withRotationRads(float rads) {
			rotation = Angles::RadsToDegs(rads);
			return *this;
		}

		constexpr DeferredDraw& withScale(float x, float y) {
			scale = vec(x, y);
			return *this;
		}

		constexpr DeferredDraw& withScale(float s) {
			return withScale(s, s);
		}

		constexpr DeferredDraw& withScale(vec v) {
			return withScale(v.x, v.y);
		}

		constexpr DeferredDraw& withColor(SDL_Color color) {
			c = color;
			hasColor = true;
			return *this;
		}

		constexpr DeferredDraw& withColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			return withColor({r,g,b,a});
		}

		void Draw() {
			if (hasColor) {
				t->color = c;
			}
			// We pass origin as rotation pivot. We could change that to a different variable.
			GPU_BlitTransformX(t, &src, Window::currentDrawTarget, dest.x, dest.y, origin.x, origin.y, rotation, scale.x, scale.y);
			if (hasColor) {
				t->color = { 255, 255, 255, 255 };
			}
		}

	protected:
		GPU_Image* t;
		vec dest;
		GPU_Rect src = {};
		float rotation = 0;
		vec scale = vec(1.f, 1.f);
		vec origin = vec(0.f, 0.f);
		SDL_Color c = {};
		bool hasColor = false;
	};

	struct Draw : DeferredDraw {

		constexpr Draw(GPU_Image* t, vec pos) : DeferredDraw(t,pos){}
		constexpr Draw(GPU_Image* t, float x, float y) : DeferredDraw(t,x,y) {}
		constexpr Draw(GPU_Image* t, const BoxBounds& destRect) : DeferredDraw(t, destRect) {} // sets scale
		Draw(const Draw&) = delete;
		~Draw() {
			DeferredDraw::Draw();
		}
	};

	struct PartialDraw
	{

		constexpr PartialDraw(GPU_Image* t, vec pos) : t(t), dest(pos) {
			withRect(0, 0, t->w, t->h);
		}
		constexpr PartialDraw(GPU_Image* t, float x, float y) : t(t), dest(x, y) {
			withRect(0, 0, t->w, t->h);
		}
		constexpr PartialDraw(GPU_Image* t, const BoxBounds& destRect) // sets scale
			: t(t), dest(destRect.TopLeft())
			, scale(destRect.width / float(t->w), destRect.height / float(t->h))
		{
			withRect(0, 0, t->w, t->h);
		}

		constexpr PartialDraw& withRect(float x, float y, float w, float h) {
			return withRect({ x, y, w, h });
		}

		constexpr PartialDraw& withColor(SDL_Color color) {
			c = color;
			hasColor = true;
			return *this;
		}

		constexpr PartialDraw& withColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			return withColor({r,g,b,a});
		}

		constexpr PartialDraw& withRect(const GPU_Rect& r) {
			src = r;
			return *this;
		}

		constexpr PartialDraw& withOrigin(float x, float y) {
			origin.x = x;
			origin.y = y;
			return *this;
		}

		constexpr PartialDraw& withOrigin(vec o) {
			return withOrigin(o.x, o.y);
		}

		constexpr PartialDraw& withRotationDegs(float degrees) {
			rotation = degrees;
			return *this;
		}

		constexpr PartialDraw& withRotationRads(float rads) {
			rotation = Angles::RadsToDegs(rads);
			return *this;
		}

		constexpr PartialDraw& withScale(float x, float y) {
			scale = vec(x, y);
			return *this;
		}

		constexpr PartialDraw& withScale(float s) {
			return withScale(s, s);
		}

		constexpr PartialDraw& withScale(vec v) {
			return withScale(v.x, v.y);
		}

		void DoDraw() {
			if (hasColor) {
				t->color = c;
			}
			// We pass origin as rotation pivot. We could change that to a different variable.
			GPU_BlitTransformX(t, &src, Window::currentDrawTarget, dest.x, dest.y, origin.x, origin.y, rotation, scale.x, scale.y);
			if (hasColor) {
				t->color = { 255, 255, 255, 255 };
			}
		}

		GPU_Image* t;
		vec dest;
		GPU_Rect src = {};
		SDL_Color c = {};
		bool hasColor = false;
		float rotation = 0;
		vec scale = vec(1.f, 1.f);
		vec origin = vec(0.f, 0.f);
	};

}
