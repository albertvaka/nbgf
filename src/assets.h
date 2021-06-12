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

	static GPU_Image* invadersTexture;
	
	static GPU_Image* backgroundTexture;
	static GPU_Image* personTexture;
	static GPU_Image* personShadowTexture;

	static TTF_Font* font_30;
	static TTF_Font* font_30_outline;

	static Shader tintShader;

};

