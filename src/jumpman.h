#pragma once

#include "vector.h"
#include "bounds.h"
#include "tilemap.h"
#include <iostream>

class JumpMan
{
public:
    JumpMan(TileMap* map);
    void Update(float GameTime);
    void Draw(sf::Sprite& spr, sf::RenderTarget& window) {
        spr.setTextureRect(sf::Rect(0, 0, 16, 16));
        spr.setPosition(pos.x-cen.x, pos.y-cen.y);
        window.draw(spr);
    }
    void Reset() {
        acc = vec(0, 0);
        vel = vec(0, 0);
    }

    Bounds bounds() {
        return Bounds(pos, siz, true);
    }

    vec pos;
    vec acc;
    vec vel;

    vec siz;
    vec cen;
    
private:
    enum { ONWALL_LEFT = -1, ONWALL_NO = 0, ONWALL_RIGHT = 1 };


    bool grounded;
    short onWall;
    float jumpTimeLeft;
    
    bool crouched;
    bool lookingLeft;

    TileMap* map;

    void ensureAnim(std::string name) {
        // TODO: Change anim and set size and center to new anim frame
        siz.x = 16;
        siz.y = 16;
        cen.x = 8;
        cen.y = 8;
    }
};

