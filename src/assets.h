#pragma once

#include <array>

#include "sound.h"
#include "shader.h"

struct GPU_Image;
typedef struct _TTF_Font TTF_Font;
typedef struct _Mix_Music Mix_Music;

struct Assets
{
	static void LoadAll();

	static GPU_Image* hospitalTexture;

	static GPU_Image* marioTexture;

	static GPU_Image* menuBgTexture;

	static GPU_Image* fogTexture;

	static TTF_Font* font_30;
	static TTF_Font* font_30_outline;

	static Shader tintShader;

	static std::array<GPU_Image*, 4> forestParallaxTextures;

};

