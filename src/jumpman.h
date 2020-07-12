#pragma once

#include "entity.h"
#include "vec.h"
#include "bounds.h"
#include "tilemap.h"
#include "partsys.h"
#include "animation.h"
#include "singleinstance.h"

struct JumpMan : Entity, SingleInstance<JumpMan>
{
    JumpMan();
    void Update(float GameTime);
    void Draw() const;
    void Reset() {
        vel = vec(0, 0);
        polvito.Clear();
        invencibleTimer = -1.f;
        bfgCooldownTimer = 0;
        jumpTimeLeft = 0.0f;
        crouchedTime = 0.0f;
        onWall = ONWALL_NO;
        dead = false;
    }

    Bounds bounds() const {
        return Bounds(pos, size, vec(size.x/2, size.y));
    }
    Bounds maxBounds() const;

    void takeDamage(const vec& pos);
    bool isInvencible() const { return invencibleTimer > 0.f; }
    bool isHit() const { return invencibleTimer > 0.1f; }

    Animation animation;

    vec size;
    
    enum : int8_t { ONWALL_LEFT = -1, ONWALL_NO = 0, ONWALL_RIGHT = 1 };

    int8_t onWall = ONWALL_NO;
    float jumpTimeLeft = 0.0f;
    float crouchedTime = 0.0f;
    bool dead = false;

    float bfgAngle;
    vec bfgPos;
    float bfgCooldownTimer = 0.f;
    float invencibleTimer = -1.f;

    bool grounded = true;
    bool crouched = false;
    bool lookingLeft = false;

    PartSys polvito;

    void InitPolvito();
    inline void DoPolvitoJump();
    inline void DoPolvitoWallJump();
    inline void DoPolvitoLand();
    inline void DoPolvitoRun(float dt, bool toTheLeft, bool doTheExtraPolvitoLikeYouKnowItsDone);

};

