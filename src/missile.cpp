#include "missile.h"

#include "anim_lib.h"
#include "rand.h"
#include "camera.h"
#include "jumpman.h"
#include "explosive.h"
#include "bat.h"
#include "collide.h"
#include "assets.h"
#include "assets_sounds.h"
#include "common_bullet.h"
#include "common_enemy.h"
#include "particles.h"

const float kRadius = 2.f;
const float kSpeed = 120.f;
const float kMaxTurnRateRads = Angles::DegsToRads(90.f);
const float kFlockAvoidanceDistance = 30.f;
const float kSmokePerSecond = 30.f;

Missile::Missile(vec position, float angleDegs)
	: CircleEntity(position, kRadius)
	, anim(AnimLib::MISSILE)
{
	vel = vec::FromAngleDegs(angleDegs, kSpeed);
	soundChannel = Assets::soundMissile.Play();
}

void Missile::Update(float dt)
{
	anim.Update(dt);

	if (exploding) {
		if (anim.IsComplete()) {
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

	if (!Camera::Bounds().Contains(pos)) {
		alive = false;
		return;
	}

	// Particles
	smokeTimer += dt;
	if (smokeTimer > 1.f/kSmokePerSecond) {
		smokeTimer = 0;
		const GPU_Rect& sprite = anim.CurrentFrameRect();
		vec rear = pos-(vel.Normalized()*sprite.w);
		rear.DebugDraw(0,255,0);
		Particles::missile.pos = rear;
		Particles::missile.AddParticle();
	}

	for (Explosive* e : Explosive::GetAll()) {
		if (e->CheckBulletCollision(Bounds())) {
			explode();
			return;
		}
	}

	if (SmallBulletTilemapCollision(this, Tile::BreakResistance::HARD_OR_LOWER)) {
		Bat::AwakeNearbyBats(pos);
		explode();
		return;
	}

	if (DamagePlayerOnCollision(Bounds())) {
		explode();
		return;
	};

}

void Missile::explode() 
{
	exploding = true;
	Sound::Stop(soundChannel);
	Assets::soundExplode.Play();
	anim.Set(AnimLib::EXPLOSION, false);
}

void Missile::Draw() const
{
	if (exploding) {
		const GPU_Rect& rect = anim.CurrentFrameRect();
		Window::Draw(Assets::scifiTexture, pos)
			.withRect(rect)
			.withScale(0.5f)
			.withOrigin(rect.w/2, rect.h/2);
	} else {
		const GPU_Rect& rect = anim.CurrentFrameRect();
		Window::Draw(Assets::spritesheetTexture, pos)
			.withRect(rect)
			.withOrigin(0.f, rect.h/2)
			.withRotationDegs(vel.AngleDegs());
	}

	// Debug-only
	pos.DebugDraw();
	CircleBounds(pos, kFlockAvoidanceDistance).DebugDraw(255,0,0);
	Bounds().DebugDraw();
}
