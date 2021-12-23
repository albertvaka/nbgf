#pragma once

#include <array>

#include "SDL_gpu.h"
#include "sound.h"
#include "musicplayer.h"
#include "shader.h"
typedef struct _TTF_Font TTF_Font;

struct Voice {
	Sound speak;
	Sound end;
};

namespace Assets
{
	extern void LoadAll();

	inline GPU_Image* warriorTexture;
	inline GPU_Image* spritesheetTexture;
	inline GPU_Image* wheelerTexture;
	inline GPU_Image* scifiTexture;
	inline GPU_Image* minotaurTexture;

	inline GPU_Image* fogTexture;
	inline GPU_Image* dialogFrameTexture;

	inline TTF_Font* font_dialog_body;
	inline TTF_Font* font_dialog_title;

	inline TTF_Font* font_30;
	inline TTF_Font* font_30_outline;

	inline Shader tintShader;
	inline Shader fogShader;
	inline Shader waveShader;
	inline Shader fadeInDiamondsShader;
	inline Shader fadeOutDiamondsShader;
	inline Shader fadeOutCircleShader;

	inline Sound soundDeath;

	inline Voice aiVoice;
	inline Voice growlyVoice;
	inline Voice sinecrushVoice;
	inline Voice yeah1Voice;
	inline Voice yeah2Voice;

	inline std::array<GPU_Image*, 1> lavaParallaxTextures;
	inline std::array<GPU_Image*, 3> forestParallaxTextures;
	inline std::array<GPU_Image*, 4> caveParallaxTextures;
	inline std::array<GPU_Image*, 4> islandParallaxTextures;

};

