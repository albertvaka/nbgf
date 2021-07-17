#pragma once

#include "entity.h"
#include "vec.h"
#include "bounds.h"
#include "animation.h"
#include "screen.h"
#include "debug.h"
#include "tile.h"
#include "singleinstance.h"
#include "raw_input.h"

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
        lastSafeTilePos = Tile::ToTiles(position);
        invencibleTimer = -1.f;
        bfgCooldownTimer = 0;
        health = maxHealth = maxHp;
        jumpTimeLeft = 0.0f;
        crouchedTime = 0.0f;
        onWall = false;
        frozen = false;
        alive = true;
        groundTile = Tile::NONE;
        canDash = true;
        dashTimer = 0.0f;
        dashCooldown = 0.0f;
        divingRestTimer = 0.f;
        justHit = false;
        jumpFromWallTimer = 0.f;
        attacking = false;
        playerAttack.alive = false;
        ScreenManager::instance()->UpdateCurrentScreen(pos);
    }

    BoxBounds Bounds() const {
        return BoxBounds(pos, size, vec(size.x/2, size.y));
    }
    BoxBounds MaxBounds() const;

    void TakeDamage(vec src);
    void DealDamage(vec target);
    void ToSafeGround();
    vec SafeGroundPos() const {
        return Tile::FromTiles(lastSafeTilePos) + vec(Tile::Size / 2, 0);
    }
    bool isInvencible() const { return invencibleTimer > 0.f /*|| dashing*/; }
    bool isHit() const { return invencibleTimer > 0.1f; }

    vec bfgOffset() {
        if (crouched) {
            return vec(0,-10);
        } else {
            return vec(0, -16);
        }
    }

    vec GetCameraTargetPos() {
        // TODO: keep the camera so you see a bit more in the direction you are going (like in https://youtu.be/AqturoCh5lM?t=3801)
        vec camPos = (pos - vec(0, 16));
        ScreenManager::instance()->ClampCameraToScreen(camPos);
        return camPos;
    }

    Animation anim;
    
    CircleEntity playerAttack;

    vec size;

    veci lastSafeTilePos;
    
    int health;
    int maxHealth;

    bool onWall = false;
    float jumpTimeLeft = 0.0f;
    float jumpFromWallTimer = 0.f;
    float crouchedTime = 0.0f;
    bool frozen = false;
    bool justHit = false;

    float bfgAngle;
    vec bfgPos;
    float bfgCooldownTimer = 0.f;
    float invencibleTimer = -1.f;

    float divingRestTimer = 0.f;
    bool diving = false;

    bool dashing = false;
    float dashTimer = 0.0f;
    float dashCooldown = 0.0f;
    
    bool attacking = false;
    bool attackingUp = false;

    Tile groundTile = Tile::NONE;
    bool crouched = false;
    bool lookingLeft = false;
    bool canDash = true;

    void UpdateMoving(float dt);

    void SaveGame(SaveState& state) const;
    void LoadGame(const SaveState& state);
};

