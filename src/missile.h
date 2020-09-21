#pragma once

#include "entity.h"
#include "animation2.h"
#include "anim_lib.h"
#include "rand.h"
#include "partsys.h"
#include "camera.h"
#include "selfregister.h"
#include "jumpman.h"
#include "assets.h"

const float kRadius = 1.f;
const float kMaxSpeed = 120.f;
const float kMaxTurnRateRads = Angles::DegsToRads(90.f);
const float kFlockAvoidanceDistance = 30.f;

struct Missile : CircleEntity, SelfRegister<Missile>
{
	bool explode = false;
	Animation2 anim;

	static inline PartSys particles = PartSys(nullptr);
	static void InitParticles() {
		if (particles.texture != nullptr) {
			return;
		}
		particles.SetTexture(Assets::marioTexture);
		particles.AddSprite({ 5, 37, 6, 6 });

		float vel = 15;
		particles.max_vel = vec(vel, vel);
		particles.min_vel = vec(-vel, -vel);
		particles.min_ttl = 0.5f;
		particles.max_ttl = 1.f;
		particles.min_interval = 0.03f;
		particles.max_interval = 0.06f;
		particles.min_scale = 0.5f;
		particles.max_scale = 0.9f;
		particles.scale_vel = -2.5f;
		particles.min_rotation = 0.f;
		particles.max_rotation = 360.f;
		particles.rotation_vel = 180.f;
		particles.alpha = 0.75f;
	}

	Missile(const vec& position, float angleDegs) 
	  : CircleEntity(position, kRadius)
	  , anim(AnimLib::MISSILE)
	{
		vel = vec::FromAngleDegs(angleDegs, kMaxSpeed);
	}

	void boom() {
		explode = true;
		anim.SetAnimation(AnimLib::EXPLOSION);
		anim.loopable = false;
	}
	void Update(float dt)
	{
		anim.Update(dt);

		if (explode) {
			if (anim.complete) {
				alive = false;
			}
			return;
		}

		vec target = JumpMan::instance()->pos;

		// Prevent missiles from merging all into one.
		// Note it's an n^2 check. We could also do without it
		// if performance is a concern.
		for(Missile* m : Missile::GetAll()) {
			if (m == this) continue;
			if (m->pos.Distance(pos) < kFlockAvoidanceDistance) {
				target = m->pos;
			}
		}

		vel = vel.RotatedToFacePositionRads(target-pos, kMaxTurnRateRads*dt);

		pos += vel * dt;

		if (!Camera::GetBounds().Contains(pos)) {
			alive = false;
		}

	}

	void Draw() const
	{
		if (explode) {
			const GPU_Rect& rect = anim.GetCurrentRect();
			Window::Draw(Assets::scifiTexture, pos)
				.withRect(rect)
				.withScale(0.5f)
				.withOrigin(rect.w/2, rect.h/2);
		} else {
			const GPU_Rect& rect = anim.GetCurrentRect();
			Window::Draw(Assets::wheelerTexture, pos)
				.withRect(rect)
				.withOrigin(0.f, rect.h/2)
				.withRotationDegs(vel.AngleDegs());
		}
		if (Debug::Draw) {
			pos.Debuggerino();
			CircleBounds(pos, kFlockAvoidanceDistance).Draw(255,0,0);
			DrawBounds();
		}
	}
};