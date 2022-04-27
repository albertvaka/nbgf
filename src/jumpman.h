#pragma once

#include "entity.h"
#include "vec.h"
#include "bounds.h"
#include "animation.h"
#include "screen.h"
#include "tile.h"
#include "singleinstance.h"

struct SaveState;

struct JumpMan : Entity, SingleInstance<JumpMan>
{
    JumpMan();
    void Update(float dt);
    void Draw() const;
    void DrawGUI() const;
    void Reset(vec position, int maxHp);

    vec CenterPos() {
        return vec(pos.x, pos.y - size.y / 2);
    }

    BoxBounds HitBoxBounds() const {
        BoxBounds b = CollisionBounds();
        b.top += 4;
        b.height -= 4;
        return b;
    }
    BoxBounds CollisionBounds() const {
        return BoxBounds(pos, size, vec(size.x / 2, size.y));
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
        ScreenManager::ClampCameraToScreen(camPos);
        return camPos;
    }

    Animation anim;
    vec size;
    bool lookingLeft;
    CircleEntity playerAttack;
    Tile groundTile;
    veci lastSafeTilePos;
    bool onWall;
    float initialJumpY;
    float jumpFromWallTimer;
    float timeAfterBeingGrounded;
    float crouchedTime;
    bool frozen;
    bool justHit;
    float invencibleTimer;
    float divingRestTimer;
    bool diving;
    bool dashing;
    float dashTimer;
    float dashCooldown;
    bool crouched;
    bool canDash;
    bool attacking;
    bool attackingUp;
    float bfgAngle;
    vec bfgPos;
    float bfgCooldownTimer;
    int health;
    int maxHealth;
    int voiceSoundChannel = -1;

    void UpdateMoving(float dt);

    void SaveGame(SaveState& state) const;
    void LoadGame(const SaveState& state);
};

