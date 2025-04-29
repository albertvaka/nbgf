#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll()
{

	spr = LoadImage("data/spritesheet.png");
	mapspr = LoadImage("data/le_map.png");
	titleSpr = LoadImage("data/Start1.png");
	titleSpr2 = LoadImage("data/Start2.png");
	creditsSpr = LoadImage("data/title.png");

	music = LoadMusic("data/transmission.ogg");

    font_30 = LoadFont("data/8bitmadness.ttf", 40);
    font_30_outline_2 = LoadFontOutline("data/8bitmadness.ttf", 40, 2);
	

}
