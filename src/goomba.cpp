#include "goomba.h"
#include "jumpman.h"
#include "collide.h"
#include "window.h"
#include "assets.h"
#include "tilemap.h"
#include "rand.h"
#include "common_enemy.h"
#include "common_tilemapcharacter.h"

constexpr const float speed = 25;
constexpr const float chargeSpeed = 100;

constexpr const float enterChargeTime = 0.35f;
constexpr const float exitChargeTime = 0.2f;

// Area in front of it that if intersects with the player will trigger a charge towards them
constexpr const vec playerNearbyArea = vec(Tile::size * 11, Tile::size * 2);

constexpr const vec size = AnimLib::GOOMBA[0].GetSize();

Goomba::Goomba(vec pos, bool isCharger)
	: CircleEntity(pos - vec(0,8), 6)
	, anim(isCharger ? AnimLib::GOOMBACHARGER : AnimLib::GOOMBA)
	, isCharger(isCharger)
{
	goingRight = Rand::OnceEach(2);
	screen = ScreenManager::instance()->FindScreenContaining(pos);
	
	//Get it to touch the ground in case it's incorrectly placed in the tilemap
	//FIXME: Find a better way to do this and that works for all entities
	for (int i = 0; i < 5 && !IsGrounded(this->pos, size, vec()); i++) {
		this->pos.y += 0.2f;
	}
}

Bounds Goomba::ChargeBounds() const
{
	Bounds chargeBounds = Bounds::fromCenter(pos, playerNearbyArea);
	chargeBounds.left += WalkDirection() * chargeBounds.width / 2;
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
		goingRight = !goingRight;
		if (state == State::CHARGING) {
			state = State::EXIT_CHARGE;
			timer = 0.f;
		}
	}

	pos.x += GetVel() * dt;
}


void Goomba::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		goingRight = pos.x < JumpMan::instance()->pos.x; // so we are facing towards the player when they enter the screem
		return;
	}

	switch (state)
	{
	case State::WALKING:
		Walk(dt);
		if (isCharger && Collide(ChargeBounds(), JumpMan::instance()->bounds()))
		{
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

	case State::CHARGING:
		Walk(dt);
		anim.Update(dt*2);
		break;
	}

	if (ReceiveDamageFromBullets(bounds())) {
		die(); //single hit
		return;
	}

	DamagePlayerOnCollision(bounds());
}


void Goomba::Draw() const
{
	pos.Debuggerino();
	GPU_Rect rect = anim.GetCurrentRect();

	vec drawPos = pos;

	if (state == State::ENTER_CHARGE)
	{
		drawPos.y -= sinf((timer / enterChargeTime) * M_PI) * Tile::size;
	}
	else if (state == State::EXIT_CHARGE)
	{ 
		drawPos.y -= sinf((timer / exitChargeTime) * M_PI) * 2;
	}

	Window::Draw(Assets::marioTexture, drawPos)
		.withRect(rect)
		.withOrigin(rect.w / 2, rect.h / 2);

	if (Debug::Draw) {
		bounds().Draw();
		ChargeBounds().Draw(255, 0, 255);
	}

}
