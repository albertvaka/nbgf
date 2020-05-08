#include "window.h"

#include "assets.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

GPU_Image* Assets::hospitalTexture;

GPU_Image* Assets::marioTexture;

GPU_Image* Assets::menuBgTexture;

GPU_Image* Assets::fogTexture;

TTF_Font* Assets::font_30;
TTF_Font* Assets::font_30_outline;

Shader Assets::tintShader;

std::array<GPU_Image*, 4> Assets::forestParallaxTextures;

GPU_Image* loadImage(const std::string& path) {

    GPU_Image* texture = GPU_LoadImage(path.c_str());
    if (!texture) {
        Debug::out << "Unable to load image '" << path.c_str() << "': " << IMG_GetError();
        return nullptr;
    }
    GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);
    GPU_SetSnapMode(texture, GPU_SNAP_NONE);
    return texture;
}

TTF_Font* loadFont(const std::string& path, int size) {
    TTF_Font* font = TTF_OpenFont(path.c_str(), size);
    if (!font) {
        Debug::out << "Unable to load font '" << path.c_str() << "': " << TTF_GetError();
    }
    return font;
}
TTF_Font* loadFontOutline(const std::string& path, int size, int outline) {
    TTF_Font* font = loadFont(path, size);
    TTF_SetFontOutline(font, outline);
    return font;
}

Mix_Music* loadMusic(const std::string& path) {
    Mix_Music* music = Mix_LoadMUS(path.c_str());
    if (!music) {
        Debug::out << "Unable to load music '" << path.c_str() << "': " << Mix_GetError();
    }
    return music;
}

void Assets::LoadAll() {

    hospitalTexture= loadImage("data/spritesheet.png");

    marioTexture= loadImage("data/mario.png");

    menuBgTexture= loadImage("data/menubg3.png");

    fogTexture= loadImage("data/fog.png");

    font_30 = loadFont("data/PressStart2P.ttf", 30);
    font_30_outline = loadFontOutline("data/PressStart2P.ttf", 30, 3);

    tintShader.Load("data/default.vert", nullptr, "data/tint.frag");

    for (size_t i = 0; i < forestParallaxTextures.size(); i++) {
        forestParallaxTextures[i]= loadImage("data/redforest_"+ std::to_string(i)+".png");
        GPU_SetWrapMode(forestParallaxTextures[i], GPU_WRAP_REPEAT, GPU_WRAP_NONE);
    }
}
