#pragma once

#include "entity.h"
#include "anim_lib.h"
#include "assets.h"
#include "person.h"


struct Arrow : BoxEntity, SelfRegister<Arrow>
{
    Animation anim;

    Person *target_p;
    float time;

    Arrow(Person *p)
        : BoxEntity(pos, vec(400, 400))
        , anim(AnimLib::ARROW)
    {
        target_p = p;
    }

    void Update(float dt) {
        time += dt;
        pos.x = target_p->pos.x;
        pos.y = target_p->pos.y - 100;
        anim.Update(dt);
    }

    Window::PartialDraw Draw() const {
        const GPU_Rect& rect = anim.CurrentFrameRect();
        return Window::PartialDraw(Assets::arrowTexture, pos)
            .withRect(rect)
            .withOrigin(rect.w / 2, rect.h)
            .withScale(vec(3, 3));
    }
};
