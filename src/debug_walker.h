#pragma once

#include "entity.h"
#include "vec.h"
#include "bounds.h"
#include "tilemap.h"
#include "animation.h"
#include "selfregister.h"

struct DebugWalker : Entity, SelfRegister<DebugWalker>
{
    void Jump();

    DebugWalker(vec pos, float velX);
    void Update(float dt);
    void Draw() const;


    BoxBounds Bounds() const {
        return BoxBounds(pos, size, vec(size.x/2, size.y));
    }

    Animation animation;

    vec size = vec(16, 32);

    enum : int8_t { ONWALL_LEFT = -1, ONWALL_NO = 0, ONWALL_RIGHT = 1 };

    int8_t onWall = ONWALL_NO;
    float jumpTimeLeft = 0.0f;

    bool grounded = true;
    bool lookingLeft = false;

};

