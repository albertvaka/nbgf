#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    spritesheetTexture = LoadImage("data/spritesheet.png");
    playerTexture = LoadImage("data/player.png");
    doctorTexture = LoadImage("data/doctor.png");
    patientIdleTexture = LoadImage("data/patient_idle.png");
    patientOverdoseTexture = LoadImage("data/patient_overdose.png");
    patientDeadTexture = LoadImage("data/patient_dead.png");
    patientNarcosisTexture = LoadImage("data/patient_narcosis.png");
    patientScreamTexture = LoadImage("data/patient_scream.png");

    screenBg = LoadImage("data/screen_bg.png");
    screenBgDamage = LoadImage("data/screen_bg_damage.png");
    screenBgDamageBlink = LoadImage("data/screen_bg_damage_blink.png");
    
    emojiDead = LoadImage("data/emoji_dead.png");
    emojiAwake = LoadImage("data/emoji_awake.png");
    emojiHigh = LoadImage("data/emoji_high.png");
    
    heart = LoadImage("data/heart.png");
    bar = LoadImage("data/patient_bar.png");
    
    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");
}
