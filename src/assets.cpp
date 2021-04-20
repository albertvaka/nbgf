#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    spritesTexture = LoadImage("data/sprite.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");

    winSnd.Load("data/win.wav");
    // dieSnd.Load("data/die.wav");
	dieSnd.Load("data/death.ogg");
    readySnd.Load("data/ready.wav");
    goSnd.Load("data/go.wav");

	shootSound.Load("data/piu.ogg");
	enemyExplosionSound.Load("data/expl.ogg");

    menuMusic = LoadMusic("data/menu.ogg");
}
