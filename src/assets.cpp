#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    spritesheetTexture = LoadImage("data/spritesheet.png");
    playerTexture = LoadImage("data/player.png");
    doctorTexture = LoadImage("data/doctor.png");
    patientIdleTexture = LoadImage("data/patient_idle.png");
    patientDeadTexture = LoadImage("data/patient_dead.png");
    patientNarcosisTexture = LoadImage("data/patient_narcosis.png");
    patientScreamTexture = LoadImage("data/patient_scream.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");
}
