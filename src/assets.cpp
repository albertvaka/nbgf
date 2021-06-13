#include "assets.h"

#include "asset_load.h"

GPU_Image* Assets::invadersTexture;
GPU_Image* Assets::backgroundTexture;
GPU_Image* Assets::personTexture;
GPU_Image* Assets::personShadowTexture;

GPU_Image* Assets::terrainTexture;

GPU_Image* Assets::treeTexture;
GPU_Image* Assets::treeShadowTexture;

GPU_Image* Assets::canisterTexture;
GPU_Image* Assets::canisterShadowTexture;

GPU_Image* Assets::bushTexture;
GPU_Image* Assets::bushShadowTexture;

GPU_Image* Assets::lightTexture;
GPU_Image* Assets::lightShadowTexture;

GPU_Image* Assets::charcoTexture;
GPU_Image* Assets::tileTexture;

GPU_Image* Assets::birdTexture;

TTF_Font* Assets::font_30;
TTF_Font* Assets::font_30_outline;


void Assets::LoadAll() {

    invadersTexture = LoadImage("data/invaders.png");
    backgroundTexture = LoadImage("data/parktest.png");
    personTexture = LoadImage("data/person_anim.png");
    personShadowTexture = LoadImage("data/shadowHard.png");
    
    terrainTexture = LoadImage("data/terrain.png");
    
    treeTexture = LoadImage("data/basic_tree_sprite.png");
    treeShadowTexture = LoadImage("data/basic_tree_shadow.png");

    canisterTexture = LoadImage("data/basic_canister_sprite.png");
    canisterShadowTexture = LoadImage("data/basic_canister_shadow.png");

    bushTexture = LoadImage("data/basic_bush_sprite.png");
    bushShadowTexture = LoadImage("data/basic_bush_shadow.png");

    lightTexture = LoadImage("data/basic_light_sprite.png");
    lightShadowTexture = LoadImage("data/basic_light_shadow.png");

    charcoTexture = LoadImage("data/basic_charco.png");
    tileTexture = LoadImage("data/basic_tile.png");
    
    birdTexture = LoadImage("data/bird.png");

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

}
