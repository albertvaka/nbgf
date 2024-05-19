#include "fireshot.h"

#include "anim_lib.h"
#include "camera.h"
#include "assets.h"
#include "common_bullet.h"
#include "common_enemy.h"


constexpr float kFireShotRadius = 2.f;
constexpr float kFireShotScale = 1.5f;

FireShot::FireShot(vec pos, vec vel, vec accel)
	: CircleEntity(pos, kFireShotRadius * kFireShotScale, vel)
	, accel(accel)
{
}

void FireShot::Update(float dt)
{
	
	vel += accel * dt;
	pos += vel * dt;
	
	if (!Camera::Bounds().Contains(pos)) {
		alive = false;
		return;
	}

	if (SmallBulletTilemapCollision(this, Tile::BreakResistance::NONE)) {
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
	Window::Draw(Assets::spritesheetTexture, pos)
		.withRect(AnimLib::FIRESHOT)
		.withScale(kFireShotScale)
		.withOrigin(AnimLib::FIRESHOT.w / 2, AnimLib::FIRESHOT.h / 2);

	// Debug-only
	Bounds().DebugDraw();
}
