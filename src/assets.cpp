#include "assets.h"

#include "asset_load.h"

GPU_Image* Assets::invadersTexture;
GPU_Image* Assets::backgroundTexture;
GPU_Image* Assets::personTexture;
GPU_Image* Assets::personShadowTexture;

GPU_Image* Assets::terrainTexture;

GPU_Image* Assets::treeTexture;
GPU_Image* Assets::treeShadowTexture;

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

    font_30 = LoadFont("data/PressStart2P.ttf", 30);
    font_30_outline = LoadFontOutline("data/PressStart2P.ttf", 30, 3);

}
