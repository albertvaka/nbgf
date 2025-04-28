#include "assets.h"

#include "musicplayer.h"
#include "window.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "asset_load.h"

void Assets::LoadAll() {

    hospitalTexture = LoadImage("data/spritesheet.png");
    marioTexture = LoadImage("data/mario.png");

    for (int i = 0; i < 3; ++i) {	
      std::string filename = "data/sun" + std::to_string(i) + ".png";	
      sunTextures[i] = LoadImage(filename);
    }
    spritesTexture = LoadImage("data/sprites.png");
    npcTexture = LoadImage("data/npc.png");
    casaTexture = LoadImage("data/bg.png");
    doggoTexture = LoadImage("data/doggo.png");
    plantTexture = LoadImage("data/plant.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

    soundDoggo1.Load("data/doggo1.ogg");
    soundDoggo2.Load("data/doggo2.ogg");
    soundDoggo3.Load("data/doggo3.ogg");

    soundBuy.Load("data/buy.ogg");
    soundSell1.Load("data/sell1.ogg");
    soundSell2.Load("data/sell2.ogg");
    soundDeath.Load("data/death.ogg");

    soundBucketTomatoes.Load("data/bucket_tomatoes.ogg");
    soundBucketWater.Load("data/bucket_water.ogg");
    soundWater.Load("data/water.ogg");
    soundPickupDrop.Load("data/pickup_drop.ogg");

    soundJump1.Load("data/jump1.ogg");
    soundJump1.SetVolume(0.50);
    soundJump2.Load("data/jump2.ogg");
    soundJump2.SetVolume(0.50);
    soundLand.Load("data/land.ogg");

    sceneMusic[0] = LoadMusic("data/plantetes_gameplay.ogg");
    sceneMusic[1] = LoadMusic("data/plantetes_menu.ogg");
    MusicPlayer::SetVolume(0.60);

    //tintShader.Load("data/default.vert", nullptr, "data/tint.frag");

}
