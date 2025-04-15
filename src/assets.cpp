#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    shipTexture = LoadImage("data/ship.png");
    heartTexture = LoadImage("data/heart.png");
    rockTexture = LoadImage("data/rocks.png");
    rockFoamTexture = LoadImage("data/rocks_foam.png");
    backgroundTexture = LoadImage("data/bg.png");
    fishTexture = LoadImage("data/fish.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");
    seaShader.Load("data/vertex_out.vert", nullptr, "data/sea.frag");
    outlineShader.Load("data/vertex_out.vert", nullptr, "data/outline.frag");

}
