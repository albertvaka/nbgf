#include "mantis.h"

#include "jumpman.h"
#include "collide.h"
#include "window.h"
#include "assets.h"
#include "shader.h"
#include "rand.h"
#include "common_tilemapcharacter.h"
#include "common_enemy.h"

constexpr const float gravity_acc = 660; // TODO: reuse from jumpman for consistency

constexpr const float speed = 60;
constexpr const float jumpSpeedY = -350;
constexpr const float maxJumpSpeedX = 300;

constexpr const float attackRadius = 310;

constexpr const float scale = 1.5f;
constexpr const float mantisHealth = 5;

// Square used to collide against the tilemap
constexpr const vec spriteSize = vec(24* scale, 24* scale);
// Radius used to collide against the player (a bit smaller)
constexpr const float spriteRadius = 10.f* scale;


constexpr const vec vel_hit(180.f, -150.f);
constexpr const float hitTime = 0.5f;

constexpr const float jumpCooldown = .2f;

Mantis::Mantis(vec pos)
	: CircleEntity(pos - vec(0,8), spriteRadius)
	, anim(AnimLib::MANTIS_WALK)
{
	screen = ScreenManager::instance()->FindScreenContaining(pos);
	initialPos = this->pos;
	initialVelX = Rand::OnceEvery(2) ? -speed : speed;
	Reset();
}

void Mantis::Reset() {
	pos = initialPos;
	health = mantisHealth;
	vel.y = 10;
	vel.x = initialVelX;
	state = State::JUMP;
}

vec Mantis::GetJumpSpeedToTarget(vec target) {
	// TODO: When pos and target are at different heights maybe the result isn't correct with this formula?
	vec displacement = pos - target;
	float speedX = (2*jumpSpeedY * displacement.x) / (gravity_acc + 2 * displacement.y);
	Mates::Clamp(speedX, -maxJumpSpeedX, maxJumpSpeedX);
	return vec(speedX, jumpSpeedY);
}

bool Mantis::IsBouncingAgainstAnotherMantis() {
	if (!collidingWith) {
		return false;
	}
	return IsMovingTowardsInX(pos, vel, collidingWith->pos);
}

void Mantis::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		if (pos != initialPos) {
			if (!Camera::Bounds().Contains(pos)) {
				Reset();
			}
		}
		return;
	}

	//Debug::out << ENUM_NAME_OF(state);

	hitTimer -= dt;
	jumpCooldownTimer -= dt;

	JumpMan* player = JumpMan::instance();

	switch (state)
	{
	case State::WALKING: 
	{
		anim.Update(dt);

		if (IsGoingToHitAWall(pos, spriteSize, vel, dt)
			|| IsGoingToRunOffPlatform(pos, spriteSize, vel, dt)
			|| IsGoingToLeaveTheScreen(pos, spriteSize, vel, dt, screen)
			|| IsBouncingAgainstAnotherMantis())
		{
			vel.x = -vel.x;
		}
		auto ret = MoveAgainstTileMap(pos, spriteSize, vel, dt);
		pos = ret.pos;

		if (jumpCooldownTimer <= 0.f && Collide(CircleBounds(pos, attackRadius), player->Bounds()))
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
		}
	}
	break;
	case State::JUMP:
	{
		vel.y += gravity_acc * dt;
		
		// Disabled temporarily since the screen where I'm testing the mantis has a screen bounds smaller than the actual 
		// space in the tilemap and makes it go crazy when it reaches that area by jumping. This code is meant to prevent
		// the mantis from leaving to the sides of the sceen, but not through the top/bottom.
		//if (IsGoingToLeaveTheScreen(pos, spriteSize, vel, dt, screen))
		//{
		//	vel.x = -vel.x;
		//}

		// Bounce against other mantis
		if (IsBouncingAgainstAnotherMantis()
			|| IsGoingToLeaveTheScreen(pos, spriteSize, vel, dt, screen)) {
			vel.x = -vel.x * 0.5f;
		}

		auto ret = MoveAgainstTileMap(pos, spriteSize, vel, dt);
		pos = ret.pos;

		if (ret.groundCollision != Tile::NONE) {
			vel.x = player->pos.x > pos.x ? speed : -speed;
			vel.y = 0;
			state = State::WALKING;
			jumpCooldownTimer = jumpCooldown;
			anim.Set(AnimLib::MANTIS_WALK);
		} else if (ret.ceilingCollision != Tile::NONE) {
			vel.y = 0;
		} else if (ret.leftWallCollision != Tile::NONE || ret.rightWallCollision != Tile::NONE) {
			vel.x = -vel.x;
		}

	}
	break;
	}

	Bullet* b = ReceiveDamageFromBullets(Bounds());
	if (b) {
		TakeDamage(b->pos);
		if (alive == false) return;
	}

	DamagePlayerOnCollision(Bounds());
}

void Mantis::TakeDamage(vec src)
{
	hitTimer = hitTime;

	if (pos.x > src.x) {
		vel.x = vel_hit.x;
	}
	else {
		vel.x = -vel_hit.x;
	}

	if (state != State::JUMP) {
		vel.y = vel_hit.y;
		state = State::JUMP;
	}

	health--;
	if (health <= 0) {
		DieWithSmallExplosion(this);
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
		.withScale(vel.x> 0? -scale : scale, scale)
		.withOrigin(rect.w / 2, rect.h / 2);

	Shader::Deactivate();

	// Debug-only
	Bounds().DebugDraw();
	BoxBounds::FromCenter(pos, spriteSize).DebugDraw();
	CircleBounds(pos, attackRadius).DebugDraw();
}
