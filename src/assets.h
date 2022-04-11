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

	inline Shader tintShader; // uniforms: flashColor
	inline Shader fogShader; //uniforms: vec offset, float time, int octaves, rgba fogColor
	inline Shader waveShader; // uniforms: vec camera, float time
	inline Shader fadeInDiamondsShader; // uniforms: float progress
	inline Shader fadeOutDiamondsShader; // uniforms: float progress
	inline Shader fadeOutCircleShader; // uniforms: float progress, vec normalizedTarget, float aspectRatio

	inline Sound soundMegazero;
	inline MultiSound<2> soundExplode;
	inline MultiSound<3> soundRobotStep;
	inline Sound soundRattle;
	inline MultiSound<2> soundVoiceAttack;
	inline MultiSound<2> soundVoiceDiveAttack;
	inline MultiSound<2> soundVoiceDamage;
	inline MultiSound<4> soundVoiceJump;
	inline MultiSound<2> soundMetallicBounce;

	inline Voice aiVoice;
	inline Voice growlyVoice;
	inline Voice sinecrushVoice;
	inline Voice yeah1Voice;
	inline Voice yeah2Voice;

	inline std::array<GPU_Image*, 3> forestParallaxTextures;
	inline std::array<GPU_Image*, 4> caveParallaxTextures;
	inline std::array<GPU_Image*, 4> islandParallaxTextures;
	inline std::array<GPU_Image*, 1> lavaParallaxTextures;
	inline std::array<GPU_Image*, 1> sewerParallaxTextures;

};

