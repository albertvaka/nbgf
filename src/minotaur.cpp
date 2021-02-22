#include "minotaur.h"
#include "jumpman.h"
#include "collide.h"
#include "window.h"
#include "assets.h"
#include "rand.h"
#include "common_enemy.h"
#include "common_tilemapcharacter.h"

constexpr const float kRunSpeed = 80;

constexpr const float kScale = 2.0f;
constexpr const float kMinotaurHealth = 9.0f;

constexpr const float kExitIdleDistance = 200.f;

constexpr const vec kMinotaurSize = vec(25, 38)*kScale;
constexpr const vec kOffsetCenter = vec(3, 3)*kScale;

Minotaur::Minotaur(vec pos)
	: BoxEntity(pos-vec(0,kMinotaurSize.y/2), kMinotaurSize)
	, anim(AnimLib::MINOTAUR_IDLE)
{
	screen = ScreenManager::instance()->FindScreenContaining(pos);
	
	//Get it to touch the ground in case it's incorrectly placed in the tilemap
	//FIXME: Find a better way to do this and that works for all entities
	for (int i = 0; i < 400 && !IsGrounded(this->pos, size, vec()); i++) { // FIXME: Change 400 to 5 when we place this guy on the ground and not on a savestation
		this->pos.y += 0.2f;
	}
	
	initialPos = this->pos;

	Reset();
}

void Minotaur::Reset()
{
	state = State::IDLE;
	anim.Ensure(AnimLib::MINOTAUR_IDLE);
	pos = initialPos;
	health = kMinotaurHealth;
	goingRight = pos.x < JumpMan::instance()->pos.x; // so we are facing towards the player when they enter the screen
}

void Minotaur::Update(float dt)
{
	if (ScreenManager::InScreenTransition) {
		return;
	}
	if (!InSameScreenAsPlayer(screen)) {
		if (pos != initialPos) {
			Reset();
		}
		return;
	}

	anim.Update(dt);

	bool wasAttacked = false;
	if (ReceiveDamageFromBullets(Bounds())) {
		health -= 1;
		wasAttacked = true;
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

	Window::Draw(Assets::minotaurTexture, pos+vec(goingRight? kOffsetCenter.x : -kOffsetCenter.x, kOffsetCenter.y))
		.withRect(rect)
		.withOrigin(rect.w / 2, rect.h / 2)
		.withScale(goingRight? kScale : -kScale, kScale);

	// Debug-only
	pos.DebugDraw();
	Bounds().DebugDraw();
	CircleBounds(pos, kExitIdleDistance).DebugDraw();
}
