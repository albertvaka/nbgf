#include "health.h"

#include "mates.h"
#include "assets.h"
#include "window.h"
#include "jumpman.h"
#include "collide.h"
#include "rand.h"

extern float mainClock;

vec Health::drawOffset() const {
	float offsetY = 2.f * sin(pos.x + pos.y + mainClock * 2.5f);
	return vec(0, offsetY);
}

void Health::Update(float dt)
{
	JumpMan* player = JumpMan::instance();
	if (Collide(Bounds(), player->Bounds())) {
		if (player->health < player->maxHealth) {
			player->health++;
		}
		alive = false;
	}


	Health::particles.pos = pos - drawOffset() + Rand::VecInRange(vec(-4, -1), vec(4, 3));
	Health::particles.SpawnWithExternalTimer(particlesTimer, dt);
}

void Health::Draw() const
{
	Window::Draw(Assets::marioTexture, pos - drawOffset())
		.withOrigin(4, 3)
		.withScale(0.7f)
		.withRotationDegs(16.f*sin(pos.y+pos.x+mainClock*5))
		.withRect(340, 21, 8, 6);

	// Debug-only
	Bounds().DebugDraw();
}

void Health::InitParticles() {
	if (particles.texture != nullptr) {
		return; // It's already been init
	}
	particles.SetTexture(Assets::marioTexture);
	particles.AddSprite({ 353, 18, 4, 4 });

	particles.max_vel = vec(12, -13);
	particles.min_vel = vec(-12, -16);
	particles.min_ttl = 0.5f;
	particles.max_ttl = 0.6f;
	particles.min_interval = 0.1f;
	particles.max_interval = 0.1f;
	particles.min_scale = 0.5f;
	particles.max_scale = 0.7f;
	particles.scale_vel = -0.5f;
	particles.min_rotation = 0.f;
	particles.max_rotation = 360.f;
	particles.rotation_vel = 180.f;
	particles.rotation_vel = 180.f;
	particles.alpha = 0.7f;
}
