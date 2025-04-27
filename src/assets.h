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
	static inline GPU_Image* heartTexture;
	static inline GPU_Image* rockTexture;
	static inline GPU_Image* rockFoamTexture;
	static inline GPU_Image* arrowTexture;
	static inline GPU_Image* particlesTexture;
	static inline GPU_Image* fishTexture;

	static inline TTF_Font* font_small;
	static inline TTF_Font* font_small_outline;
	static inline TTF_Font* font_large;
	static inline TTF_Font* font_large_outline;

	static inline Shader tintShader;
	static inline Shader seaShader;
	static inline Shader outlineShader;
};
