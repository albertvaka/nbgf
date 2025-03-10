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

	static inline GPU_Image* shipTexture;
	static inline GPU_Image* invadersTexture;

	static inline GPU_Image* backgroundTexture;

	static inline TTF_Font* font_30;
	static inline TTF_Font* font_30_outline;

	static inline Shader tintShader;

};
