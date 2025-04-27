#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    shipTexture = LoadImage("data/ship.png");
    heartTexture = LoadImage("data/heart.png");
    rockTexture = LoadImage("data/rocks.png");
    rockFoamTexture = LoadImage("data/rocks_foam.png");
    arrowTexture = LoadImage("data/arrow.png");
    particlesTexture = LoadImage("data/particles.png");
    fishTexture = LoadImage("data/fish.png");

    font_small = LoadFont("data/CherryBombOne-Regular.ttf", 80);
    font_small_outline = LoadFontOutline("data/CherryBombOne-Regular.ttf", 80, 4);
    font_large = LoadFont("data/CherryBombOne-Regular.ttf", 160);
    font_large_outline = LoadFontOutline("data/CherryBombOne-Regular.ttf", 160, 6);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");
    seaShader.Load("data/vertex_out.vert", nullptr, "data/sea.frag");
    outlineShader.Load("data/vertex_out.vert", nullptr, "data/outline.frag");

}
