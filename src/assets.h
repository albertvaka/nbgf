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
	
	static inline GPU_Image* titleTexture;

	static inline GPU_Image* seaBgTexture;
	static inline GPU_Image* shipBgTexture;

	static inline GPU_Image* spritesheetTexture;

	static inline GPU_Image* emojiSuprised;
	static inline GPU_Image* emojiDead;
	static inline GPU_Image* emojiHappy;
	static inline GPU_Image* emojiConfused;
	static inline GPU_Image* emojiLove;
	static inline GPU_Image* emojiNeutral;

	static inline GPU_Image* greenTick;
	static inline GPU_Image* heart;
	static inline GPU_Image* bar;
	static inline GPU_Image* barOutline;

	static inline TTF_Font* font_30;
	static inline TTF_Font* font_30_outline;

	static inline Shader tintShader;
	static inline Shader underwaterShader;

	static inline Mix_Music* menuMusic;

	static inline MultiSound<4> note1p1;
	static inline MultiSound<4> note2p1;
	static inline MultiSound<4> note3p1;
	static inline MultiSound<4> note4p1;

	static inline MultiSound<4> note1p2;
	static inline MultiSound<4> note2p2;
	static inline MultiSound<4> note3p2;
	static inline MultiSound<4> note4p2;
};
