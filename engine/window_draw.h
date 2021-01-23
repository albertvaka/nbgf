#pragma once

#include "SDL_gpu.h"

#include "angles.h"
#include "bounds.h"

namespace Window {

	extern GPU_Target* currentDrawTarget;

	struct Draw {

		constexpr Draw(GPU_Image* t, vec pos) : t(t), dest(pos) {
			withRect(0,0,t->w,t->h);
		}
		constexpr Draw(GPU_Image* t, float x, float y) : t(t), dest(x,y) {
			withRect(0,0,t->w,t->h);
		}
		constexpr Draw(GPU_Image* t, const BoxBounds& destRect) // sets scale
			: t(t), dest(destRect.TopLeft())
			, scale(destRect.width/float(t->w), destRect.height/float(t->h))
		{
			withRect(0,0,t->w,t->h);
		}
		Draw(const Draw&) = delete;

		constexpr Draw& withRect(float x, float y, float w, float h) {
			return withRect({ x, y, w, h });
		}

		constexpr Draw& withRect(const GPU_Rect& r) {
			src = r;
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

		constexpr Draw& withOrigin(vec o) {
			return withOrigin(o.x, o.y);
		}

		constexpr Draw& withRotationDegs(float degrees) {
			rotation = degrees;
			return *this;
		}

		constexpr Draw& withRotationRads(float rads) {
			rotation = Angles::RadsToDegs(rads);
			return *this;
		}

		constexpr Draw& withScale(float x, float y) {
			scale = vec(x, y);
			return *this;
		}

		constexpr Draw& withScale(float s) {
			return withScale(s, s);
		}

		constexpr Draw& withScale(vec v) {
			return withScale(v.x, v.y);
		}

		~Draw() {
			// We pass origin as rotation pivot. We could change that to a different variable.
			GPU_BlitTransformX(t, &src, Window::currentDrawTarget, dest.x, dest.y, origin.x, origin.y, rotation, scale.x, scale.y);
			GPU_SetRGBA(t, 255, 255, 255, 255);
		}
	
	private:
		GPU_Image* t;
		vec dest;
		GPU_Rect src = {};
		float rotation = 0;
		vec scale = vec(1.f, 1.f);
		vec origin = vec(0.f, 0.f);
	};

}
