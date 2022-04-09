#include "minotaur.h"

#include "jumpman.h"
#include "window_draw.h"
#include "assets.h"
#include "shader.h"
#include "rand.h"
#include "screen.h"
#include "common_enemy.h"
#include "common_tilemapcharacter.h"

constexpr const float kScale = 2.0f;
constexpr const vec kMinotaurSize = vec(25, 38) * kScale;

constexpr const float kRunSpeed = 80;
constexpr const int kMinotaurHealth = 9;
constexpr const float kExitIdleDistance = 200.f;
constexpr const float kDistanceAttack = 125.0f;

constexpr const vec kAttackHitbox(80, 120);
constexpr const int kAttackDamageFramesBegin = 1;
constexpr const int kAttackDamageFramesEnd = 5;
constexpr const vec kAttackHitboxOffset = vec(35,15);

constexpr const vec kFlipAttackHitbox(160, 30);
constexpr const int kFlipDamageFramesBegin = 2;
constexpr const int kFlipDamageFramesEnd = 3;
constexpr const float kFlipAttackHitboxHeight = kMinotaurSize.y/3;

Minotaur::Minotaur(vec pos)
	: BoxEntity(pos-vec(0,kMinotaurSize.y/2), kMinotaurSize)
	, anim(AnimLib::MINOTAUR_IDLE)
{
	screen = ScreenManager::instance()->FindScreenContaining(pos);

	this->pos = AlignWithGround(this->pos, size);
	initialPos = this->pos;

	Reset();
}

void Minotaur::Reset()
{
	state = State::IDLE;
	anim.Ensure(AnimLib::MINOTAUR_IDLE);
	pos = initialPos;
	hitTimer = 0.f;
	health = kMinotaurHealth;
}

BoxBounds Minotaur::AttackBounds() const {
	return BoxBounds(pos, kAttackHitbox, kAttackHitbox/2 + kAttackHitboxOffset.Mirrored(goingRight, false));
}

BoxBounds Minotaur::FlipAttackBounds() const {
	return BoxBounds(pos, kFlipAttackHitbox, kFlipAttackHitbox / 2 - vec(0, kFlipAttackHitboxHeight));
}

void Minotaur::TakeDamage() {
	hitTimer = 0.3f;
	health--;
	if (health <= 0) {
		DieWithSmallExplosion(this);
		for (int i = 0; i < 6; i++) {
			RandomlySpawnHealth(Rand::VecInRange(Bounds()), 100);
		}
	}
}

void Minotaur::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		if (pos != initialPos) {
			Reset();
		}
		// Keep animating since it's seen from another screen
		anim.Update(dt);
		return;
	}

	anim.Update(dt);

	bool wasAttacked = false;

	hitTimer  -= dt;
	const vec* damageFromPlayerPos = ReceiveDamageFromPlayer(Bounds(), hitTimer > 0.f);
	if (damageFromPlayerPos) {
		wasAttacked = true;
		TakeDamage();
		if (alive == false) return;
	}

	switch (state) {
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
		if (frame >= kAttackDamageFramesBegin && frame <= kAttackDamageFramesEnd) {
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
		if (frame >= kFlipDamageFramesBegin && frame <= kFlipDamageFramesEnd) {
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
		if (hitTimer <= 0.f && goingRight != pos.x < JumpMan::instance()->pos.x)
		{
			goingRight = !goingRight;
			state = State::FLIP;
			anim.Ensure(AnimLib::MINOTAUR_FLIP, false);
		}
		else if (hitTimer <= 0.f && Collide(JumpMan::instance()->CollisionBounds(), BoxBounds::FromCenter(pos, vec(kDistanceAttack))))
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

	if (!damageFromPlayerPos) { //avoid hitting and being hit the same frame
		DamagePlayerOnCollision(Bounds());
	}
}

void Minotaur::Draw() const
{
	// HACK
	int frame = anim.CurrentFrameNumber();
	bool animHasMotionTrail = (state == State::FLIP && frame >= kFlipDamageFramesBegin && frame <= kFlipDamageFramesEnd)
		|| (state == State::ATTACK_BIG && frame >= kAttackDamageFramesBegin && frame <= kAttackDamageFramesEnd);

	if (hitTimer > 0.f && !animHasMotionTrail) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
	}

	GPU_Rect rect = anim.CurrentFrameRect();
	Window::Draw(Assets::minotaurTexture, pos)
		.withRect(rect)
		.withOrigin(rect.w / 2, rect.h / 2)
		.withScale(goingRight? kScale : -kScale, kScale);

	Shader::Deactivate();

	Bounds().DebugDraw();
}

int Minotaur::DrawHealth(int offset) const {
	if (!InSameScreenAsPlayer(screen)) {
		return 0;
	}
	DrawBossHealth(health, kMinotaurHealth, offset);
	return 1;
}
