#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    spritesheetTexture = LoadImage("data/spritesheet.png");
    playerTexture = LoadImage("data/player.png");
    doctorTexture = LoadImage("data/doctor.png");
    patientTexture = LoadImage("data/doctor.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");
}
