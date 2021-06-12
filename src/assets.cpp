#include "assets.h"

#include "asset_load.h"

GPU_Image* Assets::invadersTexture;
GPU_Image* Assets::backgroundTexture;
GPU_Image* Assets::personTexture;
GPU_Image* Assets::personShadowTexture;

TTF_Font* Assets::font_30;
TTF_Font* Assets::font_30_outline;

Shader Assets::tintShader;

void Assets::LoadAll() {

    invadersTexture = LoadImage("data/invaders.png");
    backgroundTexture = LoadImage("data/citybg.png");
    personTexture = LoadImage("data/complete_person.png");
    personShadowTexture = LoadImage("data/shadowHard.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");
}
