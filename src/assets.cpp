#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    spritesTexture = LoadImage("data/sprite.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");

    readySnd.Load("data/ready.ogg");
    shootSound.Load("data/piu.wav");
    shootSound2.Load("data/piu2.wav");
    enemyExplosionSound.Load("data/expl.ogg");
    dieSnd.Load("data/death.ogg");

    menuMusic = LoadMusic("data/menu.ogg");
    bossMusic = LoadMusic("data/clubdiver.ogg");
    gameMusic = LoadMusic("data/kickshock.ogg");
}
