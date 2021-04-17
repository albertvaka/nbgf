#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    spritesTexture = LoadImage("data/sprite.png");
    invadersTexture = LoadImage("data/invaders.png");
    backgroundTexture = LoadImage("data/bg.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");

    winSnd.Load("data/win.wav");
    dieSnd.Load("data/die.wav");
    readySnd.Load("data/ready.wav");
    goSnd.Load("data/go.wav");
}
