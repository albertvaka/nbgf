#include "window.h"

#include "assets.h"
#include "asset_load.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

void Assets::LoadAll() {

    hospitalTexture= LoadImage("data/spritesheet.png");

    marioTexture= LoadImage("data/mario.png");

    menuBgTexture= LoadImage("data/menubg3.png");

    fogTexture= LoadImage("data/fog.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");

    for (size_t i = 0; i < forestParallaxTextures.size(); i++) {
        forestParallaxTextures[i]= LoadImage("data/redforest_"+ std::to_string(i)+".png");
        GPU_SetWrapMode(forestParallaxTextures[i], GPU_WRAP_REPEAT, GPU_WRAP_NONE);
    }
}
