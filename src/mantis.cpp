#include "mantis.h"

#include "jumpman.h"
#include "collide.h"
#include "window.h"
#include "assets.h"
#include "shader.h"
#include "tilemap.h"
#include "rand.h"
#include "common_enemy.h"

#include "magic_enum.h"
using namespace magic_enum::ostream_operators;

constexpr const float gravity_acc = 600; // shared with jumpman

constexpr const float speed = 25;
constexpr const float jumpSpeedY = -350;
constexpr const float maxJumpSpeedX = 200;

constexpr const float attackRadius = 150;

constexpr const vec spriteSize = vec(26, 24);

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
	// TODO: Calcular be quan la alçada es diferent
	vec displacement = pos - target;
	float speedX = (2*jumpSpeedY * displacement.x) / (gravity_acc + 2 * displacement.y);
	Mates::Clamp(speedX, -maxJumpSpeedX, maxJumpSpeedX);
	return vec(speedX, jumpSpeedY);
}

void Mantis::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		return;
	}

	vec newPos = pos + vel*dt;

	TileMap* tm = TileMap::instance();
	float walkDir = vel.x > 0 ? 1 : -1;

	veci tilePosFeet = TileMap::toTiles(newPos.x + spriteSize.x / 2 * walkDir, newPos.y + spriteSize.y / 2 + 3);
	const Tile tileFeet = tm->getTile(tilePosFeet);

	veci tilePosAt = TileMap::toTiles(newPos.x + spriteSize.x / 2 * walkDir, newPos.y + spriteSize.y/2 - 3);
	const Tile tileAt = tm->getTile(tilePosAt);

	veci tilePosBottom = TileMap::toTiles(newPos.x + spriteSize.x / 2 * walkDir, pos.y + spriteSize.y);
	const Tile tileBottom = tm->getTile(tilePosBottom);

	veci tilePosAbove = TileMap::toTiles(newPos.x + spriteSize.x / 2 * walkDir, newPos.y - spriteSize.y/2);
	const Tile tileAbove = tm->getTile(tilePosAbove);

	veci tilePosSide = TileMap::toTiles(newPos.x + spriteSize.x / 2 * walkDir, newPos.y);
	const Tile tileSide = tm->getTile(tilePosSide);

	//Debug::out << state;

	hitTimer -= dt;
	jumpCooldownTimer -= dt;

	JumpMan* player = JumpMan::instance();

	switch (state)
	{
	case State::WALKING:
		anim.Update(dt);

		if ((tileBottom.isFullSolid() || tileBottom.isOneWay()) && !tileSide.isSolid() && ScreenManager::instance()->ScreenBounds(screen).Contains(newPos))
		{
			pos = newPos;
		}
		else
		{
			//Debug::out << "hitwall";
			vel.x = -vel.x;
		}

		if (collidingWith) {
			if ((collidingWith->pos.x > pos.x && vel.x > 0) || (collidingWith->pos.x < pos.x && vel.x < 0)) {
				vel.x = -vel.x;
			}
		}

		if (jumpCooldownTimer <= 0.f && Collide(CircleBounds(pos, attackRadius), player->bounds()))
		{
			//Debug::out << "preparing";
			initialPlayerPosition = player->pos;
			state = State::PREPARE_JUMP;
			anim.Set(AnimLib::MANTIS_PREPARE_JUMP);
			anim.loopable = false;
		}
		break;

	case State::PREPARE_JUMP:
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
		break;

	case State::JUMP:
		vel.y += gravity_acc * dt;
		if (tileAbove.isFullSolid() && vel.y < 0) {
			vel.y = 0;
		} else if (tileSide.isSolid()) {
			vel.x = -vel.x;
			//Debug::out << "hitwall";
		}

		// Bounce against other mantis
		if (collidingWith) {
			if ((collidingWith->pos.x > pos.x && vel.x > 0) || (collidingWith->pos.x < pos.x && vel.x < 0)) {
				vel.x = -vel.x*0.5f;
				break;
			}
		}
		
		if (tileAt.isFullSolid() || (vel.y >= 0 && tileAt.isEmpty() && tileFeet.isOneWay())) {
			//Debug::out << "at solid";
			vel.x = player->pos.x > pos.x? speed : -speed;
			vel.y = 0;
			state = State::WALKING;
			jumpCooldownTimer = jumpCooldown;
			anim.Set(AnimLib::MANTIS_WALK);
			pos.y = TileMap::alignToTiles(pos.y + 8) + 4;
		}
		else {
			pos = newPos;
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
