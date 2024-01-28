#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    bgTexture = LoadImage("data/background.png");
    titleTexture = LoadImage("data/title.png");

    spritesheetTexture = LoadImage("data/spritesheet.png");
    playerTexture = LoadImage("data/player.png");
    doctorTexture = LoadImage("data/doctor.png");
    doctorHighTexture = LoadImage("data/doctor_high.png");
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
    
    greenTick = LoadImage("data/tick.png");
    heart = LoadImage("data/heart.png");

    bar = LoadImage("data/bar_inside.png");
    barOutline = LoadImage("data/bar_outline.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");

    menuMusic = LoadMusic("data/jaunty.ogg");
}
