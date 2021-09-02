#include "mantis.h"

#include "jumpman.h"
#include "collide.h"
#include "window.h"
#include "assets.h"
#include "shader.h"
#include "rand.h"
#include "particles.h"
#include "common_tilemapcharacter.h"
#include "common_enemy.h"

constexpr const float kGravityAcc = 660; // TODO: reuse from jumpman for consistency, keep in sync meanwhile

constexpr const float kSpeed = 60;
constexpr const float kJumpSpeedY = -350;
constexpr const float kMaxJumpSpeedX = 300;

constexpr const float kAttackRadius = 310;

constexpr const float kScale = 1.5f;
constexpr const float kMantisHealth = 5;

// Square used to collide against the tilemap
constexpr const vec kSpriteSize = vec(24* kScale, 24* kScale);
// Radius used to collide against the player (a bit smaller)
constexpr const float kSpriteRadius = 10.f* kScale;


constexpr const vec kKnockbackVel(180.f, -150.f);
constexpr const float kHitTime = 0.5f;

constexpr const float kJumpCooldown = .25f;
constexpr const float kJumpCooldownRand = .65f;

Mantis::Mantis(vec pos)
	: CircleEntity(pos - vec(0,8), kSpriteRadius)
	, anim(AnimLib::MANTIS_WALK)
{
	screen = ScreenManager::instance()->FindScreenContaining(pos);
	initialPos = this->pos;
	initialVelX = Rand::OnceEvery(2) ? -kSpeed : kSpeed;
	Reset();
}

void Mantis::Reset() {
	pos = initialPos;
	health = kMantisHealth;
	vel.y = 10;
	vel.x = initialVelX;
	state = State::JUMP;
}

vec Mantis::GetJumpSpeedToTarget(vec target) {
	// TODO: When pos and target are at different heights maybe the result isn't correct with this formula?
	vec displacement = pos - target;
	float speedX = (2* kJumpSpeedY * displacement.x) / (kGravityAcc + 2 * displacement.y);
	Mates::Clamp(speedX, -kMaxJumpSpeedX, kMaxJumpSpeedX);
	return vec(speedX, kJumpSpeedY);
}

bool Mantis::IsBouncingAgainstAnotherMantis() {
	if (!collidingWith) {
		return false;
	}
	if (hitTimer > 0.f || collidingWith->hitTimer > 0.f) {
		return false; // Do not bounce against hit mantis
	}
	return IsMovingTowardsInX(pos, vel, collidingWith->pos);
}

void Mantis::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		if (pos != initialPos) {
			Reset();
		}
		return;
	}

	//Debug::out << ENUM_NAME_OF(state);

	jumpCooldownTimer -= dt;
	hitTimer -= dt;
	const vec* damageFromPlayerPos = ReceiveDamageFromPlayer(Bounds(), hitTimer > 0.f);
	if (damageFromPlayerPos) {
		TakeDamage(*damageFromPlayerPos);
		if (alive == false) return;
	}

	JumpMan* player = JumpMan::instance();

	switch (state)
	{
	case State::ATTACKING:
	{
		anim.Update(dt);
		if (anim.IsComplete()) {
			state = State::WALKING;
			anim.Set(AnimLib::MANTIS_WALK);
		}
	}
	break;
	case State::WALKING: 
	{
		anim.Update(dt);

		if (IsGoingToHitAWall(pos, kSpriteSize, vel, dt)
			|| IsGoingToLeaveTheScreen(pos, kSpriteSize, vel, dt, screen)
			|| IsBouncingAgainstAnotherMantis())
		{
			vel.x = -vel.x;
		} else if (IsGoingToRunOffPlatform(pos, vec(0.f, kSpriteSize.y), vel, dt)) {
			state = State::JUMP;
			pos.y += 1.f;
			vel.y += kGravityAcc * dt;

		}

		Particles::DoDustRun(vec(pos.x, pos.y + kSpriteSize.y / 2), dt/2.f, vel.y < 0, false); // Half the dt so we do less dust

		auto ret = MoveAgainstTileMap(pos, kSpriteSize, vel, dt);
		pos = ret.pos;

		if (jumpCooldownTimer <= 0.f && Collide(CircleBounds(pos, kAttackRadius), player->Bounds()))
		{
			//Debug::out << "preparing";
			initialPlayerPosition = player->pos;
			state = State::PREPARE_JUMP;
			anim.Set(AnimLib::MANTIS_PREPARE_JUMP, false);
		}
	}
	break;
	case State::PREPARE_JUMP:
	{
		anim.Update(dt);
		if (anim.IsComplete()) {
			//Debug::out << "prepared";
			vec predictedPlayerPos = player->pos + (player->pos - initialPlayerPosition);
			//playerPosition.DebugDraw();
			//predictedPlayerPos.DebugDraw();
			//Debug::FrameByFrame = true;
			vel = GetJumpSpeedToTarget(predictedPlayerPos);
			state = State::JUMP;
			Particles::DoDustJump(vec(pos.x, pos.y + kSpriteSize.y/2));
		}
	}
	break;
	case State::JUMP:
	{
		vel.y += kGravityAcc * dt;
		
		// Disabled temporarily since the screen where I'm testing the mantis has a screen bounds smaller than the actual 
		// space in the tilemap and makes it go crazy when it reaches that area by jumping. This code is meant to prevent
		// the mantis from leaving to the sides of the sceen, but not through the top/bottom.
		//if (IsGoingToLeaveTheScreen(pos, spriteSize, vel, dt, screen))
		//{
		//	vel.x = -vel.x;
		//}

		// Bounce against other mantis
		if (IsBouncingAgainstAnotherMantis()
			|| IsGoingToLeaveTheScreen(pos, kSpriteSize, vel, dt, screen)) {
			vel.x = -vel.x * 0.5f;
		}

		auto ret = MoveAgainstTileMap(pos, kSpriteSize, vel, dt);
		pos = ret.pos;

		if (ret.groundCollision != Tile::NONE) {
			vel.x = player->pos.x > pos.x ? kSpeed : -kSpeed;
			if (hitTimer > kHitTime/2) {
				vel.x = -vel.x; // If just hit move away instead of towards the player
			}
			vel.y = 0;
			state = State::WALKING;
			Particles::DoDustLand(vec(pos.x, pos.y + kSpriteSize.y/2));
			jumpCooldownTimer = kJumpCooldown + Rand::rollf(kJumpCooldownRand);
			anim.Set(AnimLib::MANTIS_WALK);
		} else if (ret.ceilingCollision != Tile::NONE) {
			vel.y = 0;
		} else if (ret.leftWallCollision != Tile::NONE || ret.rightWallCollision != Tile::NONE) {
			if (hitTimer > 0.f) {
				vel.x = 0.f; // Do not bounce back if you hit it away
			}
			else {
				vel.x = -vel.x;
			}
		}

	}
	break;
	}

	if (!damageFromPlayerPos) { //avoid hitting and being hit the same frame
		bool hit = DamagePlayerOnCollision(Bounds());
		if (hit && state == State::WALKING) {
			vel.x = player->pos.x > pos.x ? kSpeed : -kSpeed;
			state = State::ATTACKING;
			anim.Set(AnimLib::MANTIS_ATTACK, false);
		}
	}
}

void Mantis::TakeDamage(vec src)
{
	hitTimer = kHitTime;

	if (pos.x > src.x) {
		vel.x = kKnockbackVel.x;
	}
	else {
		vel.x = -kKnockbackVel.x;
	}

	if (state != State::JUMP) {
		vel.y = kKnockbackVel.y;
		state = State::JUMP;
	}

	health--;
	if (health <= 0) {
		DieWithSmallExplosion(this);
		bool lastMantis = true;
		for (Mantis* e : Mantis::GetAll()) {
			if (e->alive) {
				lastMantis = false;
				break;
			}
		}
		if (lastMantis) {
			for (int i = 0; i < 5; i++) {
				RandomlySpawnHealth(Rand::VecInRange(Bounds().EnclosingBoxBounds()), 100);
			}
		}
	}
}

void Mantis::Draw() const
{
	if (hitTimer > 0.f) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
	}

	GPU_Rect rect = (state == State::JUMP) ? AnimLib::MANTIS_AIR : anim.CurrentFrameRect();
	Window::Draw(Assets::spritesheetTexture, pos)
		.withRect(rect)
		.withScale(vel.x> 0? -kScale : kScale, kScale)
		.withOrigin(rect.w / 2, rect.h / 2);

	Shader::Deactivate();

	// Debug-only
	Bounds().DebugDraw();
	BoxBounds::FromCenter(pos, kSpriteSize).DebugDraw();
	CircleBounds(pos, kAttackRadius).DebugDraw();
}
