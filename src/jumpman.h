#pragma once

#include "entity.h"
#include "vector.h"
#include "bounds.h"
#include "tilemap.h"
#include "partsys.h"
#include "animation.h"
#include <iostream>

struct JumpMan : Entity
{
    JumpMan(TileMap* map);
    void Update(float GameTime);
    void Draw(sf::RenderTarget& window);
    void Reset() {
        vel = vec(0, 0);
        polvito.Clear();
    }

    Bounds bounds() const {
        return Bounds(pos, siz, vec(siz.x/2, siz.y));
    }
    vec center() const {
        return pos + vec(0, -siz.y/2.f);
    }

    void takeDamage(vec pos);
    bool isInvencible() const { return invencibleTimer > 0; }
    bool isHit() const { return invencibleTimer > 0; }

    Animation animation;

    vec siz;
    vec cen;
    
    enum { ONWALL_LEFT = -1, ONWALL_NO = 0, ONWALL_RIGHT = 1 };

    short onWall = ONWALL_NO;
    float jumpTimeLeft = 0.0f;
    
    float bfgAngle;
    vec bfgPos;
    float bfgCooldownTimer = 0.f;
    float invencibleTimer = -1.f;

    bool grounded = true;
    bool crouched = false;
    bool lookingLeft = false;

    PartSys polvito;

    TileMap* map;

    void InitPolvito();
    inline void DoPolvitoJump();
    inline void DoPolvitoWallJump();
    inline void DoPolvitoLand();
    inline void DoPolvitoRun(float dt, bool toTheLeft, bool doTheExtraPolvitoLikeYouKnowItsDone);

};

