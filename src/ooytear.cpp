#include "ooytear.h"

#include "anim_lib.h"
#include "camera.h"
#include "assets.h"
#include "common_bullet.h"
#include "common_enemy.h"
#include "particles.h"

void OoyTear::Update(float dt)
{
	pos += vel * dt;
	
	anim.Update(dt);

	if (SmallBulletTilemapCollision(this, Tile::BreakResistance::NONE)) {
		alive = false;
		return;
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
