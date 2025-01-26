#include "assets.h"

#include "asset_load.h"

void Assets::LoadAll() {

    titleTexture = LoadImage("data/title.png");

    seaBgTexture = LoadImage("data/background.png");
    air = LoadImage("data/air.png");
	GPU_SetWrapMode(air, GPU_WRAP_REPEAT, GPU_WRAP_REPEAT);
    vent = LoadImage("data/vent.png");
    window = LoadImage("data/window.png");

    spritesheetTexture = LoadImage("data/spritesheet.png");
    bubbleTexture = LoadImage("data/bubbles.png");

    fish1 = LoadImage("data/body_fish1.png");
    fish1Blink = LoadImage("data/blink.png");
    fish1mouth = LoadImage("data/mouth.png");
    fish1mic = LoadImage("data/arm_mic.png");
    fish1arm = LoadImage("data/arm_move.png");
    
    fish2 = LoadImage("data/fish2_body.png");
    fish2mouth = LoadImage("data/mouth2.png");
    fish2mic = LoadImage("data/arm_mic2.png");
    fish2arm = LoadImage("data/arm_move2.png");

    emojiSuprised = LoadImage("data/emoji_surprised.png");
    emojiDead = LoadImage("data/emoji_dead.png");
    emojiHappy = LoadImage("data/emoji_happy.png");
    emojiConfused = LoadImage("data/emoji_confused.png");
    emojiLove = LoadImage("data/emoji_love.png");
    emojiNeutral = LoadImage("data/emoji_neutral.png");

    greenTick = LoadImage("data/tick.png");
    heart = LoadImage("data/heart.png");

    bar = LoadImage("data/bar_inside.png");
    barOutline = LoadImage("data/bar_outline.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    funk_30 = LoadFont("data/funk.ttf", 30);
    funk_30_outline = LoadFontOutline("data/funk.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");
    underwaterShader.Load("data/default.vert", nullptr, "data/water.frag");

    menuMusic = LoadMusic("data/funkyone.ogg");

    note1p1.LoadAsParts("data/fish/squirty_1b.ogg");
    note2p1.LoadAsParts("data/fish/squirty_2b.ogg");
    note3p1.LoadAsParts("data/fish/squirty_3b.ogg");
    note4p1.LoadAsParts("data/fish/squirty_4b.ogg");

    note1p2.LoadAsParts("data/fish/squirty_1a.ogg");
    note2p2.LoadAsParts("data/fish/squirty_2a.ogg");
    note3p2.LoadAsParts("data/fish/squirty_3a.ogg");
    note4p2.LoadAsParts("data/fish/squirty_4a.ogg");


    fadeInDiamondsShader.Load("data/vertex_out.vert", nullptr, "data/fadein_diamonds.frag");
    fadeOutDiamondsShader.Load("data/vertex_out.vert", nullptr, "data/fadeout_diamonds.frag");
    fadeOutCircleShader.Load("data/vertex_out.vert", nullptr, "data/fadeout_circle.frag");
}
