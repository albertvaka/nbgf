#pragma once

#include "entity.h"
#include "vec.h"
#include "bounds.h"
#include "tilemap.h"
#include "partsys.h"
#include "animation.h"
#include "screen.h"
#include "debug.h"
#include "singleinstance.h"

struct SaveState;

struct JumpMan : Entity, SingleInstance<JumpMan>
{
    JumpMan();
    void Update(float dt);
    void Draw() const;
    void DrawGUI() const;
    void Reset(vec position, int maxHp = 3) {
        pos = position;
        bfgPos = position + bfgOffset();
        vel = vec(0, 0);
        polvito.Clear();
        invencibleTimer = -1.f;
        bfgCooldownTimer = 0;
        health = maxHealth = maxHp;
        jumpTimeLeft = 0.0f;
        crouchedTime = 0.0f;
        onWall = ONWALL_NO;
        frozen = false;
        alive = true;
        ScreenManager::instance()->UpdateCurrentScreen(pos);
    }

    BoxBounds Bounds() const {
        return BoxBounds(pos, size, vec(size.x/2, size.y));
    }
    BoxBounds MaxBounds() const;

    void TakeDamage(vec src);
    bool isInvencible() const { return invencibleTimer > 0.f; }
    bool isHit() const { return invencibleTimer > 0.1f; }

    vec bfgOffset() {
        if (crouched) {
            return vec(0,-10);
        } else {
            return vec(0, -16);
        }
    }

    Animation animation;

    vec size;
    
    enum : int8_t { ONWALL_LEFT = -1, ONWALL_NO = 0, ONWALL_RIGHT = 1 };

    int health;
    int maxHealth;

    int8_t onWall = ONWALL_NO;
    float jumpTimeLeft = 0.0f;
    float crouchedTime = 0.0f;
    bool frozen = false;

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

    void SaveGame(SaveState& state) const;
    void LoadGame(const SaveState& state);
};

