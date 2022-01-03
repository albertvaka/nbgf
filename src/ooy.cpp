#include "ooy.h"

#include "jumpman.h"
#include "collide.h"
#include "window_draw.h"
#include "assets.h"
#include "animation.h"
#include "rand.h"
#include "common_enemy.h"
#include "common_tilemapcharacter.h"

constexpr const float idleSpeed = 25;
constexpr const float chaseSpeed = 50;

constexpr const float chaseRadius = 100.f;

constexpr const vec size = { AnimLib::OOY_IDLE.w, AnimLib::OOY_IDLE.h };

Ooy::Ooy(vec pos)
	: SteeringEntity(pos, 12.0f, 90.f, Rand::VecInRange(-10.f, 0.f, 10.f, 10.f))
{
	screen = ScreenManager::instance()->FindScreenContaining(pos);
}

void Ooy::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		return;
	}

	switch (state)
	{
	}

	if (ReceiveDamageFromPlayer(Bounds(), false)) {
		//DieWithSmallExplosion(this); //single hit
		return;
	}

	DamagePlayerOnCollision(Bounds());
}

void Ooy::Draw() const
{
	GPU_Rect rect = AnimLib::OOY_IDLE;

	Window::Draw(Assets::spritesheetTexture, pos)
		.withRectWithOriginCentered(rect);

	// Debug-only
	pos.DebugDraw();
	Bounds().DebugDraw();
	CircleBounds(pos, chaseRadius).DebugDraw(255, 0, 255);
}
