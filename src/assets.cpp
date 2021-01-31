#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    npcTexture = LoadImage("data/ppl.png");
    buildings1x1[0] = LoadImage("data/1.png");
    buildings1x1[1] = LoadImage("data/2.png");
    buildings1x1[2] = LoadImage("data/2.png"); // more common
    buildings1x1[3] = LoadImage("data/3.png");
    buildings1x1[4] = LoadImage("data/4.png");
    buildings1x1[5] = LoadImage("data/4.png"); // more common
    buildings1x1[6] = LoadImage("data/11.png");
    buildings1x1[7] = LoadImage("data/11.png"); // more common
    buildings2x1[0] = LoadImage("data/5.png");
    buildings2x1[1] = LoadImage("data/6.png");
    buildings1x2[0] = LoadImage("data/7.png");
    buildings1x2[1] = LoadImage("data/8.png");
    buildings1x2[2] = LoadImage("data/9.png");
    buildings1x2[3] = LoadImage("data/10.png");
    
    bloodTexture = LoadImage("data/blood.png");

    arrowTexture = LoadImage("data/arrow.png");
    markTexture = LoadImage("data/mark.png");

    font_30 = LoadFont("data/Raleway-BlackItalic.ttf", 90);
    font_30_outline = LoadFontOutline("data/Raleway-BlackItalic.ttf", 90, 6);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");
    
    music = LoadMusic("data/sadfaces.ogg");
    music_intro = LoadMusic("data/sadfaces_intro.ogg");

    mortIcon = LoadImage("data/mort.png");
    freezeIcon = LoadImage("data/freeze.png");
    waveIcon = LoadImage("data/wave.png");
    gathIcon = LoadImage("data/gath.png");

}
