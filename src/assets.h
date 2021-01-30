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
	inline static std::array<GPU_Image*, 8> buildings1x1;
	inline static std::array<GPU_Image*, 2> buildings2x1;
	inline static std::array<GPU_Image*, 4> buildings1x2;
	inline static GPU_Image* buildingTexture;

	inline static GPU_Image* backgroundTexture;

	inline static TTF_Font* font_30;
	inline static TTF_Font* font_30_outline;

	inline static Shader tintShader;

	inline static Mix_Music* music;
	inline static Mix_Music* music_intro;

};

