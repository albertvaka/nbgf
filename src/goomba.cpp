#include "goomba.h"

#include "jumpman.h"
#include "collide.h"
#include "window_draw.h"
#include "assets.h"
#include "rand.h"
#include "common_enemy.h"
#include "common_tilemapcharacter.h"

constexpr const float speed = 25;
constexpr const float chargeSpeed = 100;

constexpr const float enterChargeTime = 0.35f;
constexpr const float exitChargeTime = 0.2f;
constexpr const float shieldingTime = 1.1f;

// Area in front of it that if intersects with the player will trigger a charge towards them
constexpr const float detectDistanceInFront = Tile::Size * 11;
constexpr const float detectDistanceBehind = Tile::Size * 4;
constexpr const vec detectArea = vec(detectDistanceBehind + detectDistanceInFront, Tile::Size * 2);

constexpr const vec size = AnimLib::GOOMBA[0].GetSize();

Goomba::Goomba(vec pos, Type type)
	: CircleEntity(AlignWithGround(pos, Tile::Sizes), 6)
	, anim(type == Type::CHARGER? AnimLib::GOOMBACHARGER : (type == Type::SHIELDER ? AnimLib::GOOMBASHIELDER : AnimLib::GOOMBA))
	, type(type)
{
	goingRight = Rand::OnceEvery(2);
	screen = ScreenManager::instance()->FindScreenContaining(pos);
	
	this->pos = AlignWithGround(this->pos, size);
}

BoxBounds Goomba::ChargeBounds() const
{
	BoxBounds chargeBounds = BoxBounds::FromCenter(pos, detectArea);
	chargeBounds.left += WalkDirection() * (chargeBounds.width / 2 - detectDistanceBehind);
	return chargeBounds;
}

float Goomba::WalkSpeed() const
{
	if (state == State::CHARGING) 
	{
		return chargeSpeed;
	}
	return speed;
}

void Goomba::Walk(float dt)
{
	vec vel = vec(GetVel(),0);

	if (IsGoingToHitAWall(pos, size, vel, dt)
		|| IsGoingToRunOffPlatform(pos, size, vel, dt)
		|| IsGoingToLeaveTheScreen(pos, size, vel, dt, screen))
	{
		if (state == State::CHARGING) {
			state = State::EXIT_CHARGE;
			timer = 0.f;
		}
		else
		{
			goingRight = !goingRight;
		}
	}

	pos.x += GetVel() * dt;
}


void Goomba::Update(float dt)
{
	JumpMan* player = JumpMan::instance();
	if (!InSameScreenAsPlayer(screen)) {
		goingRight = pos.x < player->pos.x; // so we are facing towards the player when they enter the screem
		return;
	}

	switch (state)
	{
	case State::WALKING:
		Walk(dt);
		if ((type == Type::CHARGER || type == Type::SHIELDER) && Collide(ChargeBounds(), player->CollisionBounds()))
		{
			bool playerOnRight = (player->pos.x > pos.x);
			if (playerOnRight != goingRight) {
				goingRight = !goingRight;
			}
			state = State::ENTER_CHARGE;
			timer = 0.0f;
		}
		anim.Update(dt);
		break;

	case State::ENTER_CHARGE:
		timer += dt;
		if (timer > enterChargeTime)
		{
			state = State::CHARGING;
		}
		break;

	case State::EXIT_CHARGE:
		timer += dt;
		if (timer > exitChargeTime)
		{
			state = State::WALKING;
		}
		break;

	case State::SHIELDING:
		timer += dt;
		if (timer > shieldingTime)
		{
			state = State::WALKING;
		}
		break;
	case State::CHARGING:
		Walk(dt);
		anim.Update(dt*2);
		break;
	case State::TEST_DUMMY:
		anim.Update(dt);
		break;
	}

	const vec* hitPos = ReceiveDamageFromPlayer(Bounds(), false);
	if (hitPos) {
		if (type == Type::SHIELDER) {
			bool hitFromRight = (player->pos.x > pos.x);
			if (hitFromRight != goingRight) {
				DieWithSmallExplosion(this); //single hit
				return;
			}
			else {
				state = State::SHIELDING;
				timer = 0;
			}
		}
		else {
			DieWithSmallExplosion(this); //single hit
			return;
		}
	}

	DamagePlayerOnCollision(Bounds());
}


void Goomba::Draw() const
{
	GPU_Rect rect = anim.CurrentFrameRect();

	vec drawPos = pos;

	if (state == State::ENTER_CHARGE)
	{
		drawPos.y -= sinf((timer / enterChargeTime) * Angles::Pi) * Tile::Size;
	}
	else if (state == State::EXIT_CHARGE)
	{ 
		drawPos.y -= sinf((timer / exitChargeTime) * Angles::Pi) * 2;
	}
	else if (state == State::SHIELDING) {
		rect = AnimLib::GOOMBASHIELDING;
	}

	Window::Draw(Assets::spritesheetTexture, drawPos)
		.withRect(rect)
		.withOrigin(rect.w / 2, rect.h / 2)
		.withScale(goingRight? -1 : 1, 1);

	// Debug-only
	pos.DebugDraw();
	Bounds().DebugDraw();
	ChargeBounds().DebugDraw(255, 0, 255);
}
