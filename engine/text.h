#pragma once

#include "raylib.h"
#include <string>
#include "vec.h"

struct Text
{
	enum class MultilineAlignment {
		LEFT,
		CENTER,
		RIGHT
	};

	Text(Font& font, int fontSize, int horizontalSpacing = -1) : font(font) {
		SetFontSize(fontSize, horizontalSpacing);
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

	Text& SetFont(Font& newfont) {
		if (&newfont != &font) {
			font = newfont;
			Invalidate();
		}
		return *this;
	}

	Text& SetFontSize(int s, int horizontalSpacing = -1) {
		if (horizontalSpacing == -1) {
			horizontalSpacing = s / 10;
		}
		if (fontSize != s || hspacing != horizontalSpacing) {
			fontSize = s;
			hspacing = horizontalSpacing;
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

	Text& SetOutlineColor(uint8_t r, uint8_t g, uint8_t b) {
		if (r != outline_color.r || g != outline_color.g || b != outline_color.b) {
			outline_color.r = r;
			outline_color.g = g;
			outline_color.b = b;
			Invalidate();
		}
		return *this;
	}

	void SetMultilineAlignment(MultilineAlignment a) {
		multilineAlignment = a;
	}

	Text& SetHorizontalSpacing(int pixels) {
		if (pixels != vspacing) {
			vspacing = pixels;
			Invalidate();
		}
		return *this;
	}


	Text& SetMultilineSpacing(int pixels) {
		if (pixels != vspacing) {
			vspacing = pixels;
			Invalidate();
		}
		return *this;
	}

	Text& SetEmptyLinesMultilineSpacing(int pixels) {
		if (pixels != empty_line_vspacing) {
			empty_line_vspacing = pixels;
			Invalidate();
		}
		return *this;
	}

	vec Size() {
		if (cachedSize == vec::Zero) {
			Render(vec::Zero, true);
		}
		return cachedSize;
	}


    bool HasChanges() {
		return (cachedSize == vec::Zero);
    }

	void Render(vec position, bool measureOnly = false);

private:
	void Invalidate() {
		cachedSize = vec::Zero;
	}

	mutable vec cachedSize = vec::Zero;

	Font& font;
	int fontSize;
	Color color = { 255,255,255,255 };
	Color outline_color = { 0,0,0,255 };
	int vspacing = 0;
	int hspacing;
	int empty_line_vspacing = 12;
	MultilineAlignment multilineAlignment = MultilineAlignment::LEFT;
	std::string str;
};

struct TextColor {
	const static char MagicSeparator = 0x05;
	const static char MagicIndicator = 0x07;
	const static char Length = 6;
	constexpr uint8_t sanitize(uint8_t c) {
		// nulls shouldn't be a problem, but just in case
		if (c == 0 || c == '\n' || c == MagicSeparator) return c + 1;
		else return c;
	}
	uint8_t data[Length] = { MagicSeparator, MagicIndicator, 0,0,0, MagicSeparator };
	constexpr TextColor(uint8_t r, uint8_t g, uint8_t b) { data[2] = sanitize(r); data[3] = sanitize(g); data[4] = sanitize(b); }
	constexpr TextColor(Color color) : TextColor(color.r, color.g, color.b) {}
};

inline std::string operator+(const TextColor tc, const std::string& str)
{
	return std::string((char*)tc.data, TextColor::Length) + str;
}

inline std::string operator+(const std::string& str, const TextColor tc)
{
	return str + std::string((char*)tc.data, TextColor::Length);
}
