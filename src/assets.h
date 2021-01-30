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

	inline static GPU_Image* npcTexture;
	inline static GPU_Image* building1;
	inline static std::array<GPU_Image*, 3> buildings1x1;
	inline static GPU_Image* buildingTexture;

	inline static GPU_Image* backgroundTexture;

	inline static TTF_Font* font_30;
	inline static TTF_Font* font_30_outline;

	inline static Shader tintShader;

};

