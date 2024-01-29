#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);
    sprite = LoadImage("data/spritesheet.png");
}
