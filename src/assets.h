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


	static inline GPU_Image* hospitalTexture;

	static inline std::array<GPU_Image*, 3> sunTextures;

	static inline GPU_Image* marioTexture;

	static inline GPU_Image* spritesTexture;
	static inline GPU_Image* npcTexture;

	static inline GPU_Image* casaTexture;
	static inline GPU_Image* plantTexture;

	static inline GPU_Image* doggoTexture;

	static inline TTF_Font* font_30;
	static inline TTF_Font* font_30_outline;

	static inline Sound soundBucketTomatoes;
	static inline Sound soundBuy;
	static inline Sound soundSell1;
	static inline Sound soundSell2;
	static inline Sound soundWater;
	static inline Sound soundBucketWater;
	static inline Sound soundJump1;
	static inline Sound soundJump2;
	static inline Sound soundLand;
	static inline Sound soundPickupDrop;
	static inline Sound soundDeath;

	static inline Sound soundDoggo1;
	static inline Sound soundDoggo2;
	static inline Sound soundDoggo3;

	static inline Mix_Music* sceneMusic[2];

	//static inline Shader tintShader;
};

