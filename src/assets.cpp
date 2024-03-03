#include "assets.h"
#include "assets_sounds.h"

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

    soundMegazero.Load("data/megazero.ogg");
    soundExplode.Load({ "data/explode.ogg", "data/explodemini.ogg" });
    soundMissile.Load({ "data/rocket_launch_1.ogg", "data/rocket_launch_2.ogg" });
    soundRobotStep.Load({ "data/robotstep1.ogg","data/robotstep2.ogg","data/robotstep3.ogg" });
    soundRattle.Load("data/rattle1.ogg");
    
    soundLeaves.Load({ "data/leaves01.ogg","data/leaves02.ogg" });
    soundSword.Load({ "data/sword1.ogg","data/sword2.ogg" });
    soundStep.Load({ "data/step1.ogg","data/step2.ogg", "data/step3.ogg","data/step4.ogg" });

    soundVoiceJump.Load({ "data/voice_jump1.ogg", "data/voice_jump2.ogg", "data/voice_jump3.ogg", "data/voice_jump4.ogg" });
    soundVoiceDamage.Load({ "data/voice_damage1.ogg", "data/voice_damage2.ogg" });
    soundVoiceAttack.Load({ "data/voice_attack1.ogg", "data/voice_attack2.ogg", "data/voice_attack3.ogg"});
    soundVoiceDiveAttack.Load({ "data/voice_diveattack1.ogg", "data/voice_diveattack2.ogg" });

    soundMetallicBounce.Load({"data/clink3.ogg", "data/clink4.ogg"});
    soundSlimyDead.Load("data/slimy_monster_or_murderd_sound_6.ogg");
    soundSlimyAttack.Load("data/slimy_monster_or_murderd_sound_4.ogg");

    tintShader.Load(nullptr, nullptr, "data/tint.frag");
    //fogShader.Load("data/vertex_out.vert", nullptr, "data/fog.frag");
    waveShader.Load("data/vertex_out.vert", nullptr, "data/wave.frag");
    fadeInDiamondsShader.Load("data/vertex_out.vert", nullptr, "data/fadein_diamonds.frag");
    fadeOutDiamondsShader.Load("data/vertex_out.vert", nullptr, "data/fadeout_diamonds.frag");
    fadeOutCircleShader.Load("data/vertex_out.vert", nullptr, "data/fadeout_circle.frag");

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

    lavaParallaxTextures[0] = LoadImage("data/lava_0.png");
    GPU_SetWrapMode(lavaParallaxTextures[0], GPU_WRAP_REPEAT, GPU_WRAP_NONE);

    sewerParallaxTextures[0] = LoadImage("data/sewer.png");
    GPU_SetWrapMode(sewerParallaxTextures[0], GPU_WRAP_REPEAT, GPU_WRAP_NONE);

    aiVoice.speak.Load("data/ai_1b.ogg");
    aiVoice.end.Load("data/ai_6a.ogg");
    growlyVoice.speak.Load("data/growly_2c.ogg");
    growlyVoice.end.Load("data/ai_6c.ogg");
    sinecrushVoice.speak.Load("data/sinecrush_2b.ogg");
    sinecrushVoice.end.Load("data/sinecrush_6c.ogg");
    yeah1Voice.speak.Load("data/yeah_3a.ogg");
    yeah1Voice.end.Load("data/yeah_6b.ogg");
    yeah2Voice.speak.Load("data/yeah_3b.ogg");
    yeah2Voice.end.Load("data/yeah_6c.ogg");
}
