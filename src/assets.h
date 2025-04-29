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

	static inline GPU_Image* buttonA;
	static inline GPU_Image* buttonB;
	static inline GPU_Image* buttonX;
	static inline GPU_Image* buttonY;

	static inline GPU_Image*  spr_p1_ship;
	static inline GPU_Image*  spr_p2_ship;
	static inline GPU_Image*  spr_bg;
	static inline GPU_Image*  spr_mineral;
	static inline GPU_Image*  spr_p1_hq;
	static inline GPU_Image*  spr_p2_hq;
	static inline GPU_Image*  spr_p1_shot;
	static inline GPU_Image*  spr_p2_shot;
	static inline GPU_Image*  spr_p1_worker;
	static inline GPU_Image*  spr_p2_worker;
	static inline GPU_Image*  spr_p1_soldier;
	static inline GPU_Image*  spr_p2_soldier;
	static inline GPU_Image*  spr_mineral_carry;
	static inline GPU_Image*  spr_ship_fire;
	static inline GPU_Image*  spr_arrow_down;
	static inline GPU_Image*  spr_sheet_explosion[12];
	static inline GPU_Image*  spr_splash;

	static inline Sound sound_laser[2];
	static inline Sound sound_explosion[2];

	static inline TTF_Font* arial_30;
	static inline TTF_Font* arial_30_outline_2;

	static inline TTF_Font* font_30;
	static inline TTF_Font* font_30_outline_2;

	static inline TTF_Font* font_80;
	static inline TTF_Font* font_80_outline_2;

	static inline TTF_Font* font_160;
	static inline TTF_Font* font_160_outline_4;


};
