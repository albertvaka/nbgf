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
	static inline GPU_Image* backgroundTexture;
	static inline GPU_Image* fishTexture;

	static inline TTF_Font* font_30;
	static inline TTF_Font* font_30_outline;

	static inline Shader tintShader;
	static inline Shader seaShader;
	static inline Shader outlineShader;
};
