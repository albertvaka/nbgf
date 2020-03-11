#pragma once
#include "character.h"
#include "tilemap.h"
#include <iostream>

class JumpMan : public Character
{
public:
    JumpMan(TileMap* map);
    void Update(float GameTime);
    void Draw(sf::Sprite& spr, sf::RenderTarget& window) {
        spr.setTextureRect(sf::Rect(0, 0, 16, 16));
        spr.setPosition(pos.x, pos.y);
        window.draw(spr);
    }
private:
    static bool firstPlayer;
    bool crouched;
    bool mirror;
};

