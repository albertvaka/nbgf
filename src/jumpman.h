#pragma once

#include "vector.h"
#include "bounds.h"
#include "tilemap.h"
#include "partsys.h"
#include "animation.h"
#include <iostream>

struct JumpMan
{
    JumpMan(TileMap* map);
    void Update(float GameTime);
    void Draw(sf::Sprite& spr, sf::RenderTarget& window);
    void Reset() {
        vel = vec(0, 0);
    }

    Bounds bounds() {
        return Bounds(pos, siz, vec(siz.x/2, siz.y));
    }

    vec pos;
    vec vel;

    Animation animation;

    vec siz;
    vec cen;
    
    enum { ONWALL_LEFT = -1, ONWALL_NO = 0, ONWALL_RIGHT = 1 };

    short onWall;
    float jumpTimeLeft;
    
    bool crouched;
    bool lookingLeft;

    PartSys polvito;

    TileMap* map;

    void InitPolvito();
    inline void DoPolvitoJump();
    inline void DoPolvitoWallJump();
    inline void DoPolvitoLand();
    inline void DoPolvitoRun(float dt, vec acc, bool isTurning);

};

