#pragma once

#include "raylib.h"

#include "angles.h"
#include "bounds.h"

namespace Window {

	struct Draw {

		constexpr Draw(Texture& t, vec pos) : t(t), dest(pos) {
			withRect(0,0,t.width,t.height);
		}
		constexpr Draw(Texture& t, float x, float y) : t(t), dest(x,y) {
			withRect(0,0,t.width,t.height);
		}
		constexpr Draw(Texture& t, const BoxBounds& destRect) // sets scale
			: t(t), dest(destRect.TopLeft())
			, scale(destRect.width/float(t.width), destRect.height/float(t.height))
		{
			withRect(0,0,t.width,t.height);
		}

		constexpr Draw& withRect(float x, float y, float w, float h) {
			return withRect({ x, y, w, h });
		}

		constexpr Draw& withRect(const vec pos, const vec size) {
			return withRect({ pos.x, pos.y, size.x, size.y });
		}

		constexpr Draw& withRect(const Rectangle& r) {
			src = r;
			return *this;
		}

		constexpr Draw& withRectWithOriginCentered(float x, float y, float w, float h) {
			return withRectWithOriginCentered({ x, y, w, h });
		}

		constexpr Draw& withRectWithOriginCentered(const Rectangle& r) {
			origin.x = r.width / 2;
			origin.y = r.height / 2;
			return withRect(r);;
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
			scale.x = x;
			scale.y = y;
			return *this;
		}

		constexpr Draw& withScale(float s) {
			return withScale(s, s);
		}

		constexpr Draw& withScale(vec v) {
			return withScale(v.x, v.y);
		}

		constexpr Draw& withColor(Color color) {
			c = color;
			return *this;
		}

		constexpr Draw& withColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			return withColor({r,g,b,a});
		}

		~Draw() {
			// raylib doesn't support negative scaling, instead, to flip an image we have to 
			// specify a negative src dimension (without moving the top left corner, though!).
			// Note that dest must not flipped, which works because we multiply the negative 
			// src.width by a negative scale
			vec positiveScale = scale;
			if (scale.x < 0) {
				src.width = -src.width;
				positiveScale.x = -positiveScale.x;
			}
			if (scale.y < 0) {
				src.height = -src.height;
				positiveScale.y = -positiveScale.y;
			}
			DrawTexturePro(t, src, { dest.x, dest.y, src.width * scale.x, src.height * scale.y }, origin * positiveScale, rotation, c);
		}

	protected:
		Texture2D& t;
		vec dest;
		Rectangle src = {};
		float rotation = 0;
		vec scale = vec(1.f, 1.f);
		vec origin = vec(0.f, 0.f);
		Color c = {255,255,255,255};
	};

}
