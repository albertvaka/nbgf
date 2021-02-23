#include "minotaur.h"
#include "jumpman.h"
#include "window.h"
#include "assets.h"
#include "rand.h"
#include "common_enemy.h"
#include "common_tilemapcharacter.h"

constexpr const float kRunSpeed = 80;
constexpr const float kExitIdleDistance = 200.f;
constexpr const float kMinotaurHealth = 9.0f;

constexpr const float kScale = 2.0f;
constexpr const vec kMinotaurSize = vec(25, 38)*kScale;

Minotaur::Minotaur(vec pos)
	: BoxEntity(pos-vec(0,kMinotaurSize.y/2), kMinotaurSize)
	, anim(AnimLib::MINOTAUR_IDLE)
{
	screen = ScreenManager::instance()->FindScreenContaining(pos);

	AlignWithGround(this, size);
	initialPos = this->pos;

	Reset();
}

void Minotaur::Reset()
{
	state = State::IDLE;
	anim.Ensure(AnimLib::MINOTAUR_IDLE);
	pos = initialPos;
	health = kMinotaurHealth;
}

void Minotaur::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		if (pos != initialPos) {
			Reset();
		}
		goingRight = pos.x < JumpMan::instance()->pos.x; // so we are facing towards the player when they enter the screen
		return;
	}

	anim.Update(dt);

	bool wasAttacked = false;
	if (ReceiveDamageFromBullets(Bounds())) {
		wasAttacked = true;
		health--;
		if (health <= 0) {
			DieWithSmallExplosion(this);
			return;
		}
	}

	switch (state)
	{
	case State::IDLE:
	{
		if (wasAttacked || pos.Distance(JumpMan::instance()->pos) < kExitIdleDistance) {
			state = State::RUN;
			anim.Ensure(AnimLib::MINOTAUR_RUN);
		}
		break;
	}
	case State::RUN: 
	{
		vec vel = vec(goingRight? kRunSpeed : -kRunSpeed, 0);
		if (IsGoingToHitAWall(pos, size, vel, dt)
			|| IsGoingToRunOffPlatform(pos, size, vel, dt)
			|| IsGoingToLeaveTheScreen(pos, size, vel, dt, screen))
		{
			goingRight = !goingRight;
		}
		pos.x += vel.x * dt;
		break;
	}
	default:
		break;
	}

	DamagePlayerOnCollision(Bounds());
}


void Minotaur::Draw() const
{
	GPU_Rect rect = anim.CurrentFrameRect();

	Window::Draw(Assets::minotaurTexture, pos)
		.withRect(rect)
		.withOrigin(rect.w / 2, rect.h / 2)
		.withScale(goingRight? kScale : -kScale, kScale);

	// Debug-only
	pos.DebugDraw();
	Bounds().DebugDraw();
	CircleBounds(pos, kExitIdleDistance).DebugDraw();
}
