#pragma once

#include "entity.h"
#include "collide.h"
#include "input.h"
#include "anim_lib.h"
#include "building.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "animation.h"
#include "camera.h"

static const float scale = 0.3f;
static const float speed = 100.f;

struct Person : BoxEntity, SelfRegister<Person>
{
	mutable bool goingLeft;
	mutable Animation anim;
	Person(const vec& position)
		: BoxEntity(pos, vec(200, 400)*scale)
		, anim(AnimLib::NPC_1_DOWN)
	{
		pos = position;
	}

	void Update(float dt)
	{
		vel = vec::Zero;

		if(Input::IsPressed(0, GameKeys::UP)) {
			vel.y = -speed;
		}
		if(Input::IsPressed(0, GameKeys::RIGHT)) {
			vel.x = speed;
		}
		if(Input::IsPressed(0, GameKeys::LEFT)) {
			vel.x = -speed;
		}
		if(Input::IsPressed(0, GameKeys::DOWN)) {
			vel.y = speed;
		}

		if (vel != vec::Zero) {
			vec oldPos = pos;
			pos.x += vel.x * dt;
			for (const Building* b : Building::GetAll()) {
				if (Collide(this, b)) {
					pos.x = oldPos.x;
					vel.x = 0;
				}
			}
			pos.y += vel.y * dt;
			for (const Building* b : Building::GetAll()) {
				if (Collide(this, b)) {
					pos.y = oldPos.y;
					vel.y = 0;
				}
			}
			if (pos != oldPos) {
				anim.Update(dt);
			}
		}
	}

	void Draw() const
	{
		if (vel.x != 0) {
			anim.Ensure(AnimLib::NPC_1_LEFT);
			goingLeft = (vel.x < 0);
		} else if (vel.y != 0) {
			if (vel.y < 0) {
				anim.Ensure(AnimLib::NPC_1_UP);
			} else {
				anim.Ensure(AnimLib::NPC_1_DOWN);
			}
		}

		const GPU_Rect& rect = anim.CurrentFrameRect();
		Window::Draw(Assets::npcTexture, pos)
			.withRect(rect)
			.withOrigin(rect.w / 2, rect.h / 2)
			.withScale(goingLeft? -scale : scale, scale);

	}
};