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
	
	static inline GPU_Image* bgTexture;
	static inline GPU_Image* titleTexture;

	static inline GPU_Image* spritesheetTexture;
	static inline GPU_Image* playerTexture;
	static inline GPU_Image* doctorTexture;
	static inline GPU_Image* doctorHighTexture;
	static inline GPU_Image* patientIdleTexture;
	static inline GPU_Image* patientDeadTexture;
	static inline GPU_Image* patientOverdoseTexture;
	static inline GPU_Image* patientNarcosisTexture;
	static inline GPU_Image* patientScreamTexture;
	
	static inline GPU_Image* screenBg;
	static inline GPU_Image* screenBgDamage;
	static inline GPU_Image* screenBgDamageBlink;
	static inline GPU_Image* emojiDead;
	static inline GPU_Image* emojiAwake;
	static inline GPU_Image* emojiOverdose;
	static inline GPU_Image* emojiHigh;
	static inline GPU_Image* greenTick;
	static inline GPU_Image* heart;
	static inline GPU_Image* bar;
	static inline GPU_Image* barOutline;

	static inline TTF_Font* font_30;
	static inline TTF_Font* font_30_outline;

	static inline Shader tintShader;

};
