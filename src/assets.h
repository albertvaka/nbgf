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
	
	static inline GPU_Image* spritesheetTexture;
	static inline GPU_Image* playerTexture;
	static inline GPU_Image* doctorTexture;
	static inline GPU_Image* patientIdleTexture;
	static inline GPU_Image* patientDeadTexture;
	static inline GPU_Image* patientNarcosisTexture;
	static inline GPU_Image* patientScreamTexture;

	static inline TTF_Font* font_30;
	static inline TTF_Font* font_30_outline;

	static inline Shader tintShader;

};
