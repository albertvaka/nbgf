#pragma once

#include <array>

#include "sound.h"
#include "shader.h"

struct GPU_Image;
#include <SDL_ttf.h>
#include <SDL_mixer.h>

struct Assets
{
	static void LoadAll();

	static inline GPU_Image* spr;
	static inline GPU_Image* mapspr;
	static inline GPU_Image* titleSpr;
	static inline GPU_Image* titleSpr2;
	static inline GPU_Image* creditsSpr;

	static inline TTF_Font* font_30;
	static inline TTF_Font* font_30_outline_2;

	static inline Mix_Music* music;
};
