#include "goomba.h"
#include "jumpman.h"
#include "collide.h"
#include "window.h"
#include "assets.h"
#include "tilemap.h"
#include "rand.h"
#include "common_enemy.h"

constexpr const float speed = 25;
constexpr const float chargeSpeed = 100;

constexpr const float enterChargeTime = 0.35f;
constexpr const float exitChargeTime = 0.2f;

Goomba::Goomba(const vec& pos, bool isCharger) 
	: CircleEntity(pos - vec(0,8), 6)
	, anim(isCharger ? AnimLib::GOOMBACHARGER : AnimLib::GOOMBA)
	, isCharger(isCharger)
{
	goingRight = Rand::OnceEach(2);
	screen = ScreenManager::instance()->FindScreenContaining(pos);
}

Bounds Goomba::ChargeBounds() const
{
	Bounds chargeBounds = Bounds::fromCenter(pos, vec(Tile::size * 11, Tile::size * 2));
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

float Goomba::WalkDirection() const
{
	return (goingRight ? 1 : -1);
}

void Goomba::Walk(float dt)
{
	float realSpeed = WalkSpeed();

	float walkDir = WalkDirection();
	float new_pos_x = pos.x + dt * realSpeed * walkDir;

	TileMap* tm = TileMap::instance();

	GPU_Rect r = anim.GetCurrentRect();

	veci tilePosBottom = TileMap::toTiles(new_pos_x + r.w / 2 * walkDir, pos.y + r.h);
	const Tile tileBottom = tm->getTile(tilePosBottom);

	veci tilePosSide = TileMap::toTiles(new_pos_x + r.w / 2 * walkDir, pos.y);
	const Tile tileSide = tm->getTile(tilePosSide);

	if ((tileBottom.isFullSolid() || tileBottom.isOneWay()) && !tileSide.isSolid())
	{
		pos.x = new_pos_x;
	}
	else
	{
		goingRight = !goingRight;
		if (state == State::CHARGING) {
			state = State::EXIT_CHARGE;
			timer = 0.f;
		}
	}
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
