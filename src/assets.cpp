#include "assets.h"

#include "asset_load.h"

#include "game_constants.h"

void Assets::LoadAll() {

	arial_30 = LoadFont("data/ARIAL.TTF", 30);
	arial_30_outline_2 = LoadFontOutline("data/ARIAL.TTF", 30, 2);

	font_30 = LoadFont("data/8bitmadness.ttf", 30);
	font_30_outline_2 = LoadFontOutline("data/8bitmadness.ttf", 30, 2);

	font_80 = LoadFont("data/8bitmadness.ttf", 80);
	font_80_outline_2 = LoadFontOutline("data/8bitmadness.ttf", 80, 2);

	font_160 = LoadFont("data/8bitmadness.ttf", 160);
	font_160_outline_4 = LoadFontOutline("data/8bitmadness.ttf", 160, 4);
	
	buttonA = LoadImage("data/buttonA.png");
	buttonB = LoadImage("data/buttonB.png");
	buttonX = LoadImage("data/buttonX.png");
	buttonY = LoadImage("data/buttonY.png");

	spr_splash = LoadImage("data/splash.png");
	spr_p1_ship = LoadImage("data/p1.png");
	spr_p2_ship= LoadImage("data/p2.png");
	spr_bg= LoadImage("data/bg.png");
	spr_mineral= LoadImage("data/mineral.png");
	spr_p1_hq= LoadImage("data/hq1.png");
	spr_p2_hq= LoadImage("data/hq2.png");
	spr_p1_shot= LoadImage("data/shot1.png");
	spr_p2_shot= LoadImage("data/shot2.png");
	spr_mineral_carry= LoadImage("data/mineral.png");
	spr_p1_worker= LoadImage("data/worker1.png");
	spr_p2_worker= LoadImage("data/worker2.png");
	spr_p1_soldier= LoadImage("data/soldier1.png");
	spr_p2_soldier= LoadImage("data/soldier2.png");
	spr_ship_fire= LoadImage("data/ship_fire.png");
	spr_arrow_down= LoadImage("data/arrow_down.png");
	for (int i = 0; i < 12; ++i)
	{
		spr_sheet_explosion[i]= LoadImage("data/tank_explosion" + std::to_string(i + 1) + ".png");
	}

	sound_laser[0].Load("data/sfx_laser1.ogg");
	sound_laser[0].SetVolume(0.80);
	sound_laser[1].Load("data/sfx_laser2.ogg");
	sound_laser[1].SetVolume(0.80);
	sound_explosion[0].Load("data/explosion1.wav");
	sound_explosion[0].SetVolume(0.30);
	sound_explosion[1].Load("data/explosion2.wav");
	sound_explosion[1].SetVolume(0.30);
}
