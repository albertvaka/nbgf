#pragma once

#include "entity.h"
#include "animation2.h"
#include "anim_lib.h"
#include "rand.h"
#include "collide.h"
#include "camera.h"
#include "jumpman.h"
#include "assets.h"

const float kRadius = 5.f;
const float kMaxSpeed = 140.f;
const float kInitialSpeed = 60.f;
const float kAccel = 40.f;

struct Missile : CircleEntity, SelfRegister<Missile>
{
	bool explode = false;
	float timer_explosion = 0;
	Animation2 anim;

	Missile(const vec& position, float angleDegs) 
	  : CircleEntity(position, kRadius)
	  , anim(AnimLib::MISSILE)
	{
		vel = vec(kInitialSpeed,0).RotatedAroundOriginDegs(angleDegs);
	}

	void Update(float dt)
	{
		anim.Update(dt);

		if (explode) {
			vel = vec(0,0);
			timer_explosion += dt;
			if (timer_explosion > 1.f) {
				alive = false;
			}
			return;
		}

		vec target = JumpMan::instance()->pos;

		float speed = vel.Length();
		if (speed < kMaxSpeed) {
			speed += kAccel*dt;
			Mates::ClampMax(speed, kMaxSpeed);
			vel = vel.Normalized()* speed;
		}

		vel = vel.RotatedToFacePositionDegs(target-pos, 2*speed*dt);

		pos += vel * dt;
		if (!Camera::GetBounds().Contains(pos)) {
			alive = false;
		}

	}

	void Draw() const
	{
		if (explode) {
			// TODO
		} else {
			const GPU_Rect& rect = anim.GetCurrentRect();
			Window::Draw(Assets::wheelerTexture, pos)
				.withRect(rect)
				.withOrigin(0.f, rect.h/2)
				.withRotationDegs(vel.AngleDegs());
		}
		if (Debug::Draw) {
			pos.Debuggerino();
			DrawBounds();
		}
	}
};