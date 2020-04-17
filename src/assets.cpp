#include "assets.h"

sf::Texture Assets::hospitalTexture;
sf::Sprite Assets::hospitalSprite;

sf::Texture Assets::marioTexture;
sf::Sprite Assets::marioSprite;

sf::Texture Assets::fogTexture;

sf::Font Assets::font;

sf::Shader Assets::tintShader;

std::array<sf::Texture, 4> Assets::forestParallaxTextures;

void Assets::LoadAll() {

    hospitalTexture.loadFromFile("data/spritesheet.png");
    hospitalSprite.setTexture(hospitalTexture);

    marioTexture.loadFromFile("data/mario.png");
    marioSprite.setTexture(marioTexture);

    fogTexture.loadFromFile("data/fog.png");

    font.loadFromFile("data/PressStart2P.ttf");

    tintShader.loadFromFile("data/tint.frag", sf::Shader::Fragment);

    for (size_t i = 0; i < forestParallaxTextures.size(); i++) {
        forestParallaxTextures[i].loadFromFile("data/redforest_"+ std::to_string(i)+".png");
        forestParallaxTextures[i].setRepeated(true);
    }
}
