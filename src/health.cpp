#include "health.h"

#include "mates.h"
#include "assets.h"
#include "window_draw.h"
#include "player.h"
#include "collide.h"
#include "rand.h"
#include "particles.h"

extern float mainClock;

vec Health::drawOffset() const {
	float offsetY = 2.f * sin(pos.x + pos.y + mainClock * 2.5f);
	return vec(0, offsetY);
}

void Health::Update(float dt)
{
	Player* player = Player::instance();
	if (Collide(Bounds(), player->CollisionBounds())) {
		if (player->health < player->maxHealth) {
			player->health++;
		}
		alive = false;
	}


	Particles::health.pos = pos - drawOffset() + Rand::VecInRange(vec(-4, -1), vec(4, 3));
	Particles::health.SpawnWithExternalTimer(particlesTimer, dt);
}

void Health::Draw() const
{
	Window::Draw(Assets::spritesheetTexture, pos - drawOffset())
		.withOrigin(4, 3)
		.withScale(0.7f)
		.withRotationDegs(16.f*sin(pos.y+pos.x+mainClock*5))
		.withRect(340, 21, 8, 6);

	// Debug-only
	Bounds().DebugDraw();
}
