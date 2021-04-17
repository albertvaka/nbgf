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

	static inline GPU_Image* spritesTexture;
	static inline GPU_Image* invadersTexture;
	
	static inline GPU_Image* backgroundTexture;

	static inline TTF_Font* font_30;
	static inline TTF_Font* font_30_outline;

	static inline Shader tintShader;
	static inline Sound winSnd;
	static inline Sound dieSnd;
	static inline Sound readySnd;
	static inline Sound goSnd;

};

