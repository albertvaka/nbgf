#include "mantis.h"

#include "player.h"
#include "collide.h"
#include "window_draw.h"
#include "assets.h"
#include "shader.h"
#include "rand.h"
#include "particles.h"
#include "screen.h"
#include "common_tilemapcharacter.h"
#include "common_enemy.h"
#include "enemies_by_screen.h"

constexpr float kGravityAcc = 660; // TODO: reuse from player for consistency, keep in sync meanwhile

constexpr float kSpeed = 60;
constexpr float kJumpSpeedY = -370;
constexpr float kMaxJumpSpeedX = 270;

constexpr float kJumpRadius = 280;
constexpr float kMeleeRadius = 80;

constexpr float kScale = 1.5f;
constexpr float kMantisHealth = 5;

// Square used to collide against the tilemap
constexpr vec kBoundingBoxSize = vec(16 * kScale, 24* kScale);
// Radius used to collide against the player (a bit smaller)
constexpr float kBoundingRadius = 10.f * kScale;


constexpr vec kKnockbackVel(180.f, -150.f);
constexpr float kHitTime = 0.3f;

constexpr float kJumpCooldown = .3f;
constexpr float kJumpCooldownRand = 1.6f;

Mantis::Mantis(vec pos)
	: CircleEntity(pos - vec(0, 8), kBoundingRadius)
	, anim(AnimLib::MANTIS_WALK)
	, walkingBackwards(false)
	, screen(ScreenManager::FindScreenContaining(pos))
{
	initialPos = this->pos;
	initialVelX = Rand::OnceEvery(2) ? -kSpeed : kSpeed;
	Reset();
	EnemiesByScreen::Add(screen, this);
}

Mantis::~Mantis()
{
	EnemiesByScreen::Remove(screen, this);
}

void Mantis::Reset() {
	pos = initialPos;
	health = kMantisHealth;
	previousHealth = kMantisHealth;
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

	if (hitTimer > 0.f) {
		hitTimer -= dt;
		if (hitTimer <= 0.f) {
			previousHealth = health;
		}
	}

	std::optional<vec> damageFromPlayerPos = ReceiveDamageFromPlayer(Bounds(), hitTimer > 0.f);
	if (damageFromPlayerPos) {
		TakeDamage(*damageFromPlayerPos);
		if (alive == false) return;
	}

	Player* player = Player::instance();
	collideInnerRadius = false;
	switch (state)
	{
	case State::ATTACKING:
	{
		anim.Update(dt);
		if (anim.IsComplete()) {
			EnterWalkingState(dt);
		}
	}
	break;
	case State::WALKING: 
	{
		collideInnerRadius = Collide(CircleBounds(pos, kMeleeRadius), player->CollisionBounds());

		if (walkingBackwards && (collideInnerRadius || jumpCooldownTimer <= 0.f)) {
			walkingBackwards = false;
			vel.x = player->pos.x > pos.x ? kSpeed : -kSpeed;
		}

		if (walkingBackwards) {
			anim.UpdateReverse(dt);
		}
		else {
			anim.Update(dt);
		}

		if (IsGoingToLeaveTheScreen(pos, kBoundingBoxSize, vel, dt, screen)
			|| IsBouncingAgainstAnotherMantis())
		{
			walkingBackwards = false;
			vel.x = -vel.x;
		}

		Particles::DoDustRun(vec(pos.x, pos.y + kBoundingBoxSize.y / 2), dt/2.f, vel.y < 0, false); // Half the dt so we do less dust

		vec finalVel = vel;
		if (collideInnerRadius) {
			finalVel.x *= 1.5f;
		}
		auto ret = MoveAgainstTileMap(pos, kBoundingBoxSize, finalVel, dt);
		pos = ret.pos;
		if (!ret.leftWallCollision.isEmpty() || !ret.rightWallCollision.isEmpty()) {
			walkingBackwards = false;
			vel.x = -vel.x;
		}
		if (ret.groundCollision == Tile::NONE) {
			walkingBackwards = false;
			state = State::JUMP;
			pos.y += 1.f;
			vel.y += kGravityAcc * dt;
		}

		if (!ret.groundCollision.isSlope() && !collideInnerRadius && jumpCooldownTimer <= 0.f && Collide(CircleBounds(pos, kJumpRadius), player->CollisionBounds()))
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
			Particles::DoDustJump(vec(pos.x, pos.y + kBoundingBoxSize.y/2));
		}
	}
	break;
	case State::JUMP:
	{
		vel.y += kGravityAcc * dt;

		// Bounce against other mantis
		if (IsBouncingAgainstAnotherMantis()
			|| IsGoingToLeaveTheScreen(pos, kBoundingBoxSize, vel, dt, screen)) {
			vel.x = -vel.x * 0.5f;
		}

		auto ret = MoveAgainstTileMap(pos, kBoundingBoxSize, vel, dt);
		pos = ret.pos;

		if (ret.groundCollision != Tile::NONE) {
			EnterWalkingState(dt);
			Particles::DoDustLand(vec(pos.x, pos.y + kBoundingBoxSize.y/2));
			jumpCooldownTimer = kJumpCooldown + Rand::rollf(kJumpCooldownRand);
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
			walkingBackwards = false;
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

	bool flip = vel.x > 0;
	if (walkingBackwards) {
		flip = !flip;
	}

	GPU_Rect rect = (state == State::JUMP) ? AnimLib::MANTIS_AIR : anim.CurrentFrameRect();
	if (collideInnerRadius) {
		rect.y += AnimLib::MANTIS_WALK->rect.h;
	}
	Window::Draw(Assets::spritesheetTexture, pos)
		.withRect(rect)
		.withScale(flip? -kScale : kScale, kScale)
		.withOrigin(rect.w / 2, rect.h / 2);

	Shader::Deactivate();

	// Debug-only
	Bounds().DebugDraw();
	BoxBounds::FromCenter(pos, kBoundingBoxSize).DebugDraw();
	CircleBounds(pos, kJumpRadius).DebugDraw(COLOR_UINT8_RGB_YELLOW);
	CircleBounds(pos, kMeleeRadius).DebugDraw(COLOR_UINT8_RGB_RED);
}

int Mantis::DrawHealth(int offset) const {
	if (!InSameScreenAsPlayer(screen)) {
		return 0;
	}
	DrawBossHealth(health, previousHealth, kMantisHealth, offset);
	return 1;
}

void Mantis::EnterWalkingState(float dt) {
	Player* player = Player::instance();
	vel.x = player->pos.x > pos.x ? kSpeed : -kSpeed;
	walkingBackwards = false;
	if (hitTimer > kHitTime / 2 || Rand::OnceEvery(4)) {
		walkingBackwards = true;
		vel.x = -vel.x; // If just hit move away instead of towards the player
	}
	vel.y = kGravityAcc*dt;
	state = State::WALKING;
	anim.Set(AnimLib::MANTIS_WALK);
}
