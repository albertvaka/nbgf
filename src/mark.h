#pragma once

#include "entity.h"
#include "anim_lib.h"
#include "assets.h"
#include "person.h"

extern float mainClock;

struct Mark : BoxEntity, SelfRegister<Mark>
{
    bool planted;

    Mark(vec pos)
        : BoxEntity(pos, vec(200, 200))
    {
        planted = false;
    }

    void Update(float dt) {
        if (planted) return;
        for (auto p : Person::GetAll()) {
            if (p->player_id != -1 && Collide(this, p)) {
                planted = true;
            }
        }
    }

    void Draw() const {
        Window::Draw(Assets::markTexture, pos)
            .withOrigin(Assets::markTexture->w/2, Assets::markTexture->h/2)
            .withRotationRads(2*mainClock)
            .withScale(vec(2, 2));
    }
};
