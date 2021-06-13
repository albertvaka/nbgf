#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    warriorTexture = LoadImage("data/warrior.png");
    spritesheetTexture = LoadImage("data/spritesheet.png");
    wheelerTexture = LoadImage("data/boss_wheeler.png"); // unused
    scifiTexture = LoadImage("data/scifi.png");
    minotaurTexture = LoadImage("data/minotaur.png");

    fogTexture = LoadImage("data/fog.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    soundDeath.Load("data/death.ogg");

    tintShader.Load(nullptr, nullptr, "data/tint.frag");
    //fogShader.Load("data/vertex_out.vert", nullptr, "data/fog.frag");
    waveShader.Load("data/vertex_out.vert", nullptr, "data/wave.frag");
    fadeInDiamondsShader.Load("data/vertex_out.vert", nullptr, "data/fadein_diamonds.frag");
    fadeOutDiamondsShader.Load("data/vertex_out.vert", nullptr, "data/fadeout_diamonds.frag");
    fadeOutCircleShader.Load("data/vertex_out.vert", nullptr, "data/fadeout_circle.frag");

    for (size_t i = 0; i < lavaParallaxTextures.size(); i++) {
        lavaParallaxTextures[i] = LoadImage("data/lava_"+ std::to_string(i)+".png");
        GPU_SetWrapMode(lavaParallaxTextures[i], GPU_WRAP_REPEAT, GPU_WRAP_NONE);
    }

    for (size_t i = 0; i < forestParallaxTextures.size(); i++) {
        forestParallaxTextures[i] = LoadImage("data/redforest_"+ std::to_string(i)+".png");
        GPU_SetWrapMode(forestParallaxTextures[i], GPU_WRAP_REPEAT, GPU_WRAP_NONE);
    }

    for (size_t i = 0; i < caveParallaxTextures.size(); i++) {
        caveParallaxTextures[i] = LoadImage("data/cave_"+ std::to_string(i)+".png");
        GPU_SetWrapMode(caveParallaxTextures[i], GPU_WRAP_REPEAT, GPU_WRAP_NONE);
    }

    for (size_t i = 0; i < islandParallaxTextures.size(); i++) {
        islandParallaxTextures[i] = LoadImage("data/island_" + std::to_string(i) + ".png");
        GPU_SetWrapMode(islandParallaxTextures[i], GPU_WRAP_REPEAT, GPU_WRAP_NONE);
    }

}
