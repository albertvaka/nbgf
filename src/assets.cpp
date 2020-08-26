#include "assets.h"

#include "window.h"
#include "asset_load.h"

void Assets::LoadAll() {

    blankTexture = Window::CreateTexture(32,32);
    Window::BeginRenderToTexture(blankTexture, false);
    Window::Clear(255,255,0);
    Window::EndRenderToTexture();

    hospitalTexture = LoadImage("data/spritesheet.png");

    marioTexture = LoadImage("data/mario.png");

    menuBgTexture = LoadImage("data/menubg3.png");

    fogTexture = LoadImage("data/fog.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load(nullptr, nullptr, "data/tint.frag");
    fogShader.Load("data/vertex_out.vert", nullptr, "data/fog.frag");
    waveShader.Load("data/vertex_out.vert", nullptr, "data/wave.frag");
    fadeInDiamondsShader.Load("data/vertex_out.vert", nullptr, "data/fadein_diamonds.frag");
    fadeOutDiamondsShader.Load("data/vertex_out.vert", nullptr, "data/fadeout_diamonds.frag");

    for (size_t i = 0; i < forestParallaxTextures.size(); i++) {
        forestParallaxTextures[i]= LoadImage("data/redforest_"+ std::to_string(i)+".png");
        GPU_SetWrapMode(forestParallaxTextures[i], GPU_WRAP_REPEAT, GPU_WRAP_NONE);
    }
}
