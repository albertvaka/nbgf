#include "text.h"

#include <vector>
#include "debug.h"

SDL_Surface* Text::Render(SDL_Color fg) {
	SDL_Surface* fg_surface = TTF_RenderUTF8_Blended(font, str.c_str(), fg);
	if (!fg_surface) {
		Debug::out << "Unable to create fg text surface. SDL Error:" << TTF_GetError();
		return nullptr; // Note this will crash the program
	}
	if (!font_outline) {
		return fg_surface;
	}

	SDL_Surface* bg_surface = TTF_RenderUTF8_Blended(font_outline, str.c_str(), outline_color);
	if (!bg_surface) {
		Debug::out << "Unable to create bg text surface. SDL Error:" << TTF_GetError();
		return nullptr; // Note this will crash the program
	}

	int outline = TTF_GetFontOutline(font_outline);
	SDL_Rect rect = { outline, outline, fg_surface->w, fg_surface->h };

	// blit text onto its outline
	SDL_SetSurfaceBlendMode(fg_surface, SDL_BLENDMODE_BLEND);
	SDL_BlitSurface(fg_surface, NULL, bg_surface, &rect);
	SDL_FreeSurface(fg_surface);

	return bg_surface;
}

SDL_Surface* Text::MultiLineRender() {
	typedef std::vector<SDL_Surface*> TextRow;
	std::vector<TextRow> rows;
	int totalHeight = 0;
	int maxWidth = 0;
	std::stringstream rowss(str);
	SDL_Color fg = color;
	while (std::getline(rowss, str, '\n')) {
		if (str.empty()) {
			rows.push_back(TextRow());
			totalHeight += empty_line_spacing;
			continue;
		}
		TextRow row;
		int maxHeight = 0;
		int totalWidth = 0;
		std::stringstream columnss(str);
		while (std::getline(columnss, str, TextColor::MagicSeparator)) {
			if (str.size() == 4 && str[0] == TextColor::MagicIndicator) { // Did we find a color indicator?
				fg = { (Uint8)str[1], (Uint8)str[2], (Uint8)str[3] };
				continue;
			}
			SDL_Surface* s = Render(fg);
			if (s) {
				if (s->h > maxHeight) {
					maxHeight = s->h;
				}
				totalWidth += s->w;
				row.push_back(s);
			}
		}
		if (totalWidth > maxWidth) {
			maxWidth = totalWidth;
		}
		totalHeight += maxHeight + spacing;
		rows.push_back(row);
	}

	if (rows.size() == 1 && rows[0].size() == 1 && rows[0][0]) {
		return rows[0][0];
	}

	if (totalHeight == 0 || maxWidth == 0) {
		Debug::out << "warning: trying to render an empty string";
		if (totalHeight == 0) {
			totalHeight = 1;
		}
		if (maxWidth == 0) {
			maxWidth = 1;
		}
	}
	SDL_Surface* final = SDL_CreateRGBSurfaceWithFormat(0, maxWidth, totalHeight, 32, SDL_PIXELFORMAT_ARGB8888);
	totalHeight = 0;
	for (TextRow& row : rows) {
		if (row.empty()) {
			totalHeight += empty_line_spacing;
			continue;
		}

		int rowWidth = 0;
		int rowHeight = 0;
		for (SDL_Surface* surface : row) {
			rowWidth += surface->w;
			if (surface->h > rowHeight) {
				rowHeight = surface->h;
			};
		}

		int leftPad;
		switch (multilineAlignment) {
		default:
		case MultilineAlignment::LEFT:
			leftPad = 0;
			break;
		case MultilineAlignment::CENTER:
			leftPad = (maxWidth - rowWidth) / 2;
			break;
		case MultilineAlignment::RIGHT:
			leftPad = (maxWidth - rowWidth);
			break;
		}

		int x = leftPad;
		for (SDL_Surface* surface : row) {
			SDL_Rect dest = { x, totalHeight, surface->w, surface->h };
			SDL_BlitSurface(surface, NULL, final, &dest);
			x += surface->w;
			SDL_FreeSurface(surface);
		}
		totalHeight += rowHeight + spacing;

	}

	return final;
}

