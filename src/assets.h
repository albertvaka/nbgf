#pragma once

#include <array>

#include "SDL_gpu.h"
#include "sound.h"
#include "shader.h"
typedef struct _TTF_Font TTF_Font;

namespace Assets
{
	extern void LoadAll();

	inline GPU_Image* blankTexture;

	inline GPU_Image* hospitalTexture;

	inline GPU_Image* marioTexture;
	inline GPU_Image* wheelerTexture;
	inline GPU_Image* mantisTexture;
	inline GPU_Image* scifiTexture;

	inline GPU_Image* menuBgTexture;

	inline GPU_Image* fogTexture;

	inline TTF_Font* font_30;
	inline TTF_Font* font_30_outline;

	inline Shader tintShader;
	inline Shader fogShader;
	inline Shader waveShader;
	inline Shader fadeInDiamondsShader;
	inline Shader fadeOutDiamondsShader;

	inline std::array<GPU_Image*, 3> forestParallaxTextures;
	inline std::array<GPU_Image*, 4> caveParallaxTextures;

};

