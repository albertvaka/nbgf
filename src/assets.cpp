#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    invadersTexture = LoadTexture("data/invaders.png");
    backgroundTexture = LoadTexture("data/bg.png");

    font_30 = LoadFont("data/PressStart2P.ttf");
}
