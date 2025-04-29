#pragma once

#include "SDL_gpu.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "vec.h"

struct Text
{
	enum class MultilineAlignment {
		LEFT,
		CENTER,
		RIGHT
	};

	Text(TTF_Font* font, TTF_Font* font_outline = nullptr) : font(font), font_outline(font_outline) {}
	~Text() {
		if (cached) { 
			GPU_FreeImage(cached); 
		}
	}

	Text& SetString(const std::string& newstr) {
		if (newstr != str) {
			str = newstr;
			Invalidate();
		}
		return *this;
	}

	std::string GetString() {
		return str;
	}

	Text& SetFont(TTF_Font* newfont, TTF_Font* newfont_outline = nullptr) {
		if (newfont != font || font_outline != newfont_outline) {
			font_outline = newfont_outline;
			font = newfont;
			Invalidate();
		}
		return *this;
	}

	Text& SetFillColor(uint8_t r, uint8_t g, uint8_t b) {
		if (r != color.r || g != color.g || b != color.b) {
			color.r = r;
			color.g = g;
			color.b = b;
			Invalidate();
		}
		return *this;
	}

	Text& SetFillColor(SDL_Color c) {
		return SetFillColor(c.r, c.g, c.b);
	}

	Text& SetOutlineColor(uint8_t r, uint8_t g, uint8_t b) {
		if (r != outline_color.r || g != outline_color.g || b != outline_color.b) {
			outline_color.r = r;
			outline_color.g = g;
			outline_color.b = b;
			Invalidate();
		}
		return *this;
	}

	Text& SetOutlineColor(SDL_Color c) {
		return SetOutlineColor(c.r, c.g, c.b);
	}

	void SetMultilineAlignment(MultilineAlignment a) {
		multilineAlignment = a;
	}

	Text& SetMultilineSpacing(int pixels) {
		if (pixels != spacing) {
			spacing = pixels;
			Invalidate();
		}
		return *this;
	}

	Text& SetEmptyLinesMultilineSpacing(int pixels) {
		if (pixels != empty_line_spacing) {
			empty_line_spacing = pixels;
			Invalidate();
		}
		return *this;
	}

	vec Size() const {
		GPU_Image* image = AsImage();
		return vec(image->w, image->h);
	}

	operator GPU_Image* () const {
		return AsImage();
	}

	GPU_Image* AsImage() const {
		if (cached == nullptr) {
			SDL_Surface* surface = const_cast<Text*>(this)->MultiLineRender();
			cached = GPU_CopyImageFromSurface(surface);
			GPU_SetImageFilter(cached, GPU_FILTER_NEAREST);
			GPU_SetSnapMode(cached, GPU_SNAP_NONE);
			SDL_FreeSurface(surface);
			if (!cached) {
				printf("Unable to create text texture. SDL Error: %s\n", SDL_GetError());
			}
		}
		return cached;
	};

    bool HasChanges() {
        return (cached == nullptr);
    }

private:
	void Invalidate() {
		if (cached) {
			GPU_FreeImage(cached);
			cached = nullptr;
		}
	}

	SDL_Surface* Render(SDL_Color color);
	SDL_Surface* MultiLineRender();

	TTF_Font* font;
	TTF_Font* font_outline;
	SDL_Color color = { 255,255,255,255 };
	SDL_Color outline_color = { 0,0,0,255 };
	int spacing = 0;
	int empty_line_spacing = 12;
	mutable GPU_Image* cached = nullptr;
	MultilineAlignment multilineAlignment = MultilineAlignment::LEFT;
	std::string str;
};

struct TextColor {
	const static char MagicSeparator = 0x05;
	const static char MagicIndicator = 0x07;
	const static char Length = 6;
	constexpr Uint8 sanitize(Uint8 c) {
		// nulls shouldn't be a problem, but just in case
		if (c == 0 || c == '\n' || c == MagicSeparator) return c + 1;
		else return c;
	}
	Uint8 data[Length] = { MagicSeparator, MagicIndicator, 0,0,0, MagicSeparator };
	constexpr TextColor(Uint8 r, Uint8 g, Uint8 b) { data[2] = sanitize(r); data[3] = sanitize(g); data[4] = sanitize(b); }
	constexpr TextColor(SDL_Color color) : TextColor(color.r, color.g, color.b) {}
};

inline std::string operator+(const TextColor tc, const std::string& str)
{
	return std::string((char*)tc.data, TextColor::Length) + str;
}

inline std::string operator+(const std::string& str, const TextColor tc)
{
	return str + std::string((char*)tc.data, TextColor::Length);
}
