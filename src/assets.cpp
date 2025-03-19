#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    shipTexture = LoadImage("data/ship.png");
    invadersTexture = LoadImage("data/invaders.png");
    backgroundTexture = LoadImage("data/bg.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");
    seaShader.Load("data/vertex_out.vert", nullptr, "data/sea.frag");
}
