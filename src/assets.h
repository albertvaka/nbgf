#pragma once

#include <array>

#include "sound.h"
#include "shader.h"

struct GPU_Image;
typedef struct _TTF_Font TTF_Font;
typedef struct _Mix_Music Mix_Music;

namespace Assets
{
	extern void LoadAll();

	inline GPU_Image* hospitalTexture;

	inline GPU_Image* marioTexture;

	inline GPU_Image* menuBgTexture;

	inline GPU_Image* fogTexture;

	inline TTF_Font* font_30;
	inline TTF_Font* font_30_outline;

	inline Shader tintShader;

	inline std::array<GPU_Image*, 4> forestParallaxTextures;

};

