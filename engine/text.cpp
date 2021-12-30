#include "text.h"

#include <vector>
#include "debug.h"

SDL_Surface* Text::Render() {
	SDL_Surface* fg_surface = TTF_RenderUTF8_Blended(font, str.c_str(), color);
	if (!fg_surface) {
		printf("Unable to create text surface. SDL Error: %s\n", TTF_GetError());
		return nullptr; // Note this will crash the program
	}
	if (!font_outline) {
		return fg_surface;
	}

	SDL_Surface* bg_surface = TTF_RenderUTF8_Blended(font_outline, str.c_str(), outline_color);
	if (!bg_surface) {
		printf("Unable to create text surface. SDL Error: %s\n", TTF_GetError());
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
	std::vector<SDL_Surface*> surfaces;
	int totalHeight = 0;
	int maxWidth = 0;
	std::stringstream ss(str);
	while (std::getline(ss, str, '\n')) {
		if (str.empty()) {
			surfaces.push_back(NULL);
			totalHeight += empty_line_spacing;
			continue;
		}
		SDL_Surface* s = Render();
		totalHeight += s->h + spacing;
		if (s->w > maxWidth) {
			maxWidth = s->w;
		}
		surfaces.push_back(s);
	}

	if (surfaces.size() == 1 && surfaces[0]) {
		return surfaces[0];
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
	for (SDL_Surface* surface : surfaces) {
		if (!surface) {
			totalHeight += empty_line_spacing;
			continue;
		}

		int leftPad;
		switch (multilineAlignment) {
		default:
		case MultilineAlignment::LEFT:
			leftPad = 0;
			break;
		case MultilineAlignment::CENTER:
			leftPad = (maxWidth - surface->w) / 2;
			break;
		case MultilineAlignment::RIGHT:
			leftPad = (maxWidth - surface->w);
			break;
		}
		SDL_Rect dest = { leftPad, totalHeight, surface->w, surface->h };
		totalHeight += surface->h + spacing;
		SDL_BlitSurface(surface, NULL, final, &dest);
		SDL_FreeSurface(surface);
	}

	return final;
}

