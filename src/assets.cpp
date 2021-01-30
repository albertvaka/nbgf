#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    npcTexture = LoadImage("data/npc1.png");
    building1 = LoadImage("data/1.png");
    buildings1x1[0] = LoadImage("data/2.png");
    buildings1x1[1] = LoadImage("data/3.png");
    buildings1x1[2] = LoadImage("data/4.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");
}
