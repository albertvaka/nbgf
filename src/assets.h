#pragma once

#include <array>

#include <raylib.h>
#include "sound.h"

struct Assets
{
	static void LoadAll();

	static inline Texture2D invadersTexture;
	
	static inline Texture2D backgroundTexture;

	static inline Font font_30;
};
