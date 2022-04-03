#include "ooytear.h"

#include "anim_lib.h"
#include "camera.h"
#include "assets.h"
#include "common_bullet.h"
#include "common_enemy.h"
#include "particles.h"

OoyTear::OoyTear(vec pos, vec vel)
	: CircleEntity(pos, kOoyTearRadius, vel)
	, anim(AnimLib::OOY_TEAR)
{
	anim.current_frame = Rand::roll(AnimLib::OOY_TEAR.size());
	startedInsideWall = SmallBulletTilemapCollision(this, Tile::BreakResistance::NONE);
}

void OoyTear::Update(float dt)
{
	pos += vel * dt;
	
	anim.Update(dt);

	if (SmallBulletTilemapCollision(this, Tile::BreakResistance::NONE)) {
		if (!startedInsideWall) {
			alive = false;
			return;
		}
	}
	else {
		startedInsideWall = false;
	}

	if (DamagePlayerOnCollision(Bounds())) {
		alive = false;
		return;
	};

	// Particles
	particlesTimer += dt;
	Particles::ooyTearTrail.pos = pos;
	Particles::ooyTearTrail.SpawnWithExternalTimer(particlesTimer, dt);
}

void OoyTear::Draw() const
{
	Window::Draw(Assets::spritesheetTexture, pos)
		.withRectWithOriginCentered(anim.CurrentFrameRect())
		.withColor(255,255,255,128);

	// Debug-only
	Bounds().DebugDraw();
}
