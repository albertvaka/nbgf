#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    npcTexture = LoadImage("data/npc1.png");
    buildingTexture = LoadImage("data/building.png");
    invadersTexture = LoadImage("data/invaders.png");
    backgroundTexture = LoadImage("data/bg.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");
}
