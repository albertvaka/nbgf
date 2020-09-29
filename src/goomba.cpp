#include "goomba.h"
#include "jumpman.h"
#include "collide.h"
#include "window.h"
#include "assets.h"
#include "tilemap.h"
#include "rand.h"
#include "collide.h"
#include "bat.h"
#include "bullet.h"

Goomba::Goomba(const vec& pos, bool isCharger) 
	: CircleEntity(pos - vec(0,8), 6)
	, anim(isCharger ? AnimLib::GOOMBACHARGER : AnimLib::GOOMBA)
	, isCharger(isCharger)
{
	goingRight = Rand::OnceEach(2);
}

const float speed = 25;
const float chargeSpeed = 100;

const float enterChargeTime = 0.35f;
const float exitChargeTime = 0.2f;


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
	JumpMan* player = JumpMan::instance();

	switch (state)
	{
	case State::WALKING:
		Walk(dt);
		if (isCharger && Collide(ChargeBounds(), player->bounds()))
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

	//FIXME: Duplicated from Bat
	// Take damage from bullets
	for (Bullet* b : Bullet::GetAll()) {
		if (b->explode) continue;
		if (Collide(bounds(), b->bounds())) {
			b->pos = pos;
			b->explode = true;
			alive = false;
			Bat::AwakeNearbyBats(pos);
			return;
		}
	}

	// Damage player
	if (!player->isInvencible()) {
		if (Collide(player->bounds(), bounds())) {
			player->takeDamage(pos);
		}
	}
	//ENDFIXME
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
