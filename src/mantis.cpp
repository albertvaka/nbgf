#include "mantis.h"

#include "jumpman.h"
#include "collide.h"
#include "window.h"
#include "assets.h"
#include "shader.h"
#include "tilemap.h"
#include "rand.h"
#include "common_tilemapcharacter.h"
#include "common_enemy.h"

#include "magic_enum.h"
using namespace magic_enum::ostream_operators;

constexpr const float gravity_acc = 600; // shared with jumpman

constexpr const float speed = 25;
constexpr const float jumpSpeedY = -350;
constexpr const float maxJumpSpeedX = 200;

constexpr const float attackRadius = 150;

constexpr const vec spriteSize = vec(24, 24);

constexpr const vec vel_hit(180.f, -150.f);
constexpr const float hitTime = 0.5f;

constexpr const float jumpCooldown = .35f;

Mantis::Mantis(const vec& pos) 
	: CircleEntity(pos - vec(0,8), 10)
	, state(State::JUMP)
	, anim(AnimLib::MANTIS_WALK)
{
	vel.y = 10;
	vel.x = Rand::OnceEach(2)? -speed : speed;
	screen = ScreenManager::instance()->FindScreenContaining(pos);
}

vec Mantis::GetJumpSpeedToTarget(const vec& target) {
	// TODO: Calcular be quan la al�ada es diferent
	vec displacement = pos - target;
	float speedX = (2*jumpSpeedY * displacement.x) / (gravity_acc + 2 * displacement.y);
	Mates::Clamp(speedX, -maxJumpSpeedX, maxJumpSpeedX);
	return vec(speedX, jumpSpeedY);
}

bool Mantis::IsBouncingAgainstAnotherMantis() {
	if (!collidingWith) {
		return false;
	}
	return (collidingWith->pos.x > pos.x && vel.x > 0) || (collidingWith->pos.x < pos.x&& vel.x < 0);
}

void Mantis::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		return;
	}

	TileMap* tm = TileMap::instance();
	float walkDir = vel.x > 0 ? 1 : -1;

	//Debug::out << state;

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

		if (jumpCooldownTimer <= 0.f && Collide(CircleBounds(pos, attackRadius), player->bounds()))
		{
			//Debug::out << "preparing";
			initialPlayerPosition = player->pos;
			state = State::PREPARE_JUMP;
			anim.Set(AnimLib::MANTIS_PREPARE_JUMP);
			anim.loopable = false;
		}
	}
	break;
	case State::PREPARE_JUMP:
	{
		anim.Update(dt);
		if (anim.complete) {
			//Debug::out << "prepared";
			vec predictedPlayerPos = player->pos + (player->pos - initialPlayerPosition);
			//playerPosition.Debuggerino();
			//predictedPlayerPos.Debuggerino();
			//Debug::FrameByFrame = true;
			vel = GetJumpSpeedToTarget(predictedPlayerPos);
			state = State::JUMP;
		}
	}
	break;
	case State::JUMP:
	{
		vel.y += gravity_acc * dt;
		
		// Disabled temporarily since the screen where I' testing the mantis has a screen bounds smaller than the actual 
		// space in the tilemap and makes it go crazy when it reaches that area by jumping. This code is meant to prevent
		// the mantis from leaving to the sides of the sceen, but not through the top/bottom.
		//if (IsGoingToLeaveTheScreen(pos, spriteSize, vel, dt, screen))
		//{
		//	vel.x = -vel.x;
		//}

		// Bounce against other mantis
		if (IsBouncingAgainstAnotherMantis()) {
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

	Bullet* b = ReceiveDamageFromBullets(bounds());
	if (b) {
		hitTimer = hitTime;

		vec src = b->pos;
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
	}

	DamagePlayerOnCollision(bounds());
}


void Mantis::Draw() const
{
	pos.Debuggerino();
	GPU_Rect rect = (state == State::JUMP)? AnimLib::MANTIS_AIR : anim.GetCurrentRect();

	vec drawPos = pos;

	pos.Debuggerino();

	if (hitTimer > 0.f) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
	}

	Window::Draw(Assets::marioTexture, drawPos)
		.withRect(rect)
		.withScale(vel.x> 0? -1: 1, 1)
		.withOrigin(rect.w / 2, rect.h / 2);

	Shader::Deactivate();

	if (Debug::Draw) {
		bounds().Draw();
		CircleBounds(pos, attackRadius).Draw();
	}

}