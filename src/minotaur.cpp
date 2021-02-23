#include "minotaur.h"
#include "jumpman.h"
#include "window.h"
#include "assets.h"
#include "rand.h"
#include "common_enemy.h"
#include "common_tilemapcharacter.h"

constexpr const float kScale = 2.0f;
constexpr const vec kMinotaurSize = vec(25, 38) * kScale;

constexpr const float kRunSpeed = 80;
constexpr const int kMinotaurHealth = 9;
constexpr const float kExitIdleDistance = 200.f;
constexpr const float kDistanceAttack = 115.0f;

constexpr const vec kAttackHitbox(50, 130);
constexpr const int kAttackDamageFramesBegin = 1;
constexpr const int kAttackDamageFramesEnd = 5;
constexpr const float kAttackHitboxDistance = 60;

constexpr const vec kFlipAttackHitbox(160, 30);
constexpr const int kFlipDamageFramesBegin = 2;
constexpr const int kFlipDamageFramesEnd = 4;
constexpr const float kFlipAttackHitboxHeight = kMinotaurSize.y/3;

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

BoxBounds Minotaur::AttackBounds() const {
	return BoxBounds(pos, kAttackHitbox, kAttackHitbox/2 - vec(goingRight? kAttackHitboxDistance : -kAttackHitboxDistance, 0));
}

BoxBounds Minotaur::FlipAttackBounds() const {
	return BoxBounds(pos, kFlipAttackHitbox, kFlipAttackHitbox / 2 - vec(0, kFlipAttackHitboxHeight));
}

void Minotaur::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		if (pos != initialPos) {
			Reset();
		}
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
		goingRight = pos.x < JumpMan::instance()->pos.x; // face the player
		if (wasAttacked || pos.Distance(JumpMan::instance()->pos) < kExitIdleDistance) {
			state = State::RUN;
			anim.Ensure(AnimLib::MINOTAUR_RUN);
		}
		CircleBounds(pos, kExitIdleDistance).DebugDraw(0, 255, 255);
		break;
	}
	case State::ATTACK_BIG:
	{
		int frame = anim.CurrentFrameNumber();
		if (frame > kAttackDamageFramesBegin && frame < kAttackDamageFramesEnd) {
			DamagePlayerOnCollision(AttackBounds());
			AttackBounds().DebugDraw();
		}
		if (anim.complete) {
			state = State::RUN;
			anim.Ensure(AnimLib::MINOTAUR_RUN);
		}
		break;
	}
	case State::FLIP:
	{
		int frame = anim.CurrentFrameNumber();
		if (frame > kFlipDamageFramesBegin && frame < kFlipDamageFramesEnd) {
			DamagePlayerOnCollision(FlipAttackBounds());
			FlipAttackBounds().DebugDraw();
		}
		if (anim.complete) {
			state = State::RUN;
			anim.Ensure(AnimLib::MINOTAUR_RUN);
		}
		break;
	}
	case State::TAUNT:
	case State::RUN: 
	{
		if (state == State::TAUNT) {
			if (!anim.complete) {
				break;
			}
			state = State::RUN;
			anim.Ensure(AnimLib::MINOTAUR_RUN);
			// Continue executing State::RUN
		}
		BoxBounds::FromCenter(pos, vec(kDistanceAttack)).DebugDraw(0, 255, 255);
		if (goingRight != pos.x < JumpMan::instance()->pos.x)
		{
			goingRight = !goingRight;
			state = State::FLIP;
			anim.Ensure(AnimLib::MINOTAUR_FLIP, false);
		}
		else if (Collide(JumpMan::instance()->Bounds(), BoxBounds::FromCenter(pos, vec(kDistanceAttack))))
		{
			state = State::ATTACK_BIG;
			anim.Ensure(AnimLib::MINOTAUR_ATTACK_BIG, false);
		}
		else
		{
			vec vel = vec(goingRight ? kRunSpeed : -kRunSpeed, 0);
			if (IsGoingToHitAWall(pos, size, vel, dt)
				|| IsGoingToRunOffPlatform(pos, size, vel, dt)
				|| IsGoingToLeaveTheScreen(pos, size, vel, dt, screen))
			{
				state = State::TAUNT;
				anim.Ensure(Rand::OnceEvery(5) ? AnimLib::MINOTAUR_TAUNT : AnimLib::MINOTAUR_IDLE, false);
			}
			else
			{
				pos.x += vel.x * dt;
			}
		}
		break;
	}
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

	Bounds().DebugDraw();
}
