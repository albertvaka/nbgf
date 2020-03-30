#pragma once

#include "assets.h"

sf::Texture Assets::hospitalTexture;
sf::Sprite Assets::hospitalSprite;

sf::Texture Assets::marioTexture;
sf::Sprite Assets::marioSprite;

sf::Font Assets::font;

void Assets::LoadAll() {

    hospitalTexture.loadFromFile("data/spritesheet.png");
    hospitalSprite.setTexture(hospitalTexture);

    marioTexture.loadFromFile("data/mario.png");
    marioSprite.setTexture(marioTexture);

    font.loadFromFile("data/PressStart2P.ttf");

}
