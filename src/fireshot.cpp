#include "fireshot.h"

#include "anim_lib.h"
#include "camera.h"
#include "assets.h"
#include "common_bullet.h"
#include "common_enemy.h"

void FireShot::Update(float dt)
{
	
	vel += accel * dt;
	pos += vel * dt;
	
	if (!Camera::Bounds().Contains(pos)) {
		alive = false;
		return;
	}

	if (SmallBulletTilemapCollision(this, false)) {
		alive = false;
		return;
	}

	if (DamagePlayerOnCollision(Bounds())) {
		alive = false;
		return;
	};

}

void FireShot::Draw() const
{
	Window::Draw(Assets::marioTexture, pos)
		.withRect(AnimLib::FIRESHOT)
		.withOrigin(AnimLib::FIRESHOT.w / 2, AnimLib::FIRESHOT.h / 2);

	// Debug-only
	Bounds().DebugDraw();
}
