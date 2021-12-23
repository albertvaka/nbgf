#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    warriorTexture = LoadImage("data/warrior.png");
    spritesheetTexture = LoadImage("data/spritesheet.png");
    wheelerTexture = LoadImage("data/boss_wheeler.png"); // unused
    scifiTexture = LoadImage("data/scifi.png");
    minotaurTexture = LoadImage("data/minotaur.png");

    fogTexture = LoadImage("data/fog.png");
    dialogFrameTexture = LoadImage("data/dialog_frame.png");

    font_dialog_body = LoadFont("data/PressStart2P.ttf", 24);
    font_dialog_title = LoadFont("data/PressStart2P.ttf", 28);
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

    aiVoice.speak.Load("data/ai_1b.wav");
    aiVoice.end.Load("data/ai_6a.wav");
    growlyVoice.speak.Load("data/growly_2c.wav");
    growlyVoice.end.Load("data/ai_6c.wav");
    sinecrushVoice.speak.Load("data/sinecrush_2b.wav");
    sinecrushVoice.end.Load("data/sinecrush_6c.wav");
    yeah1Voice.speak.Load("data/yeah_3a.wav");
    yeah1Voice.end.Load("data/yeah_6b.wav");
    yeah2Voice.speak.Load("data/yeah_3b.wav");
    yeah2Voice.end.Load("data/yeah_6c.wav");
}
