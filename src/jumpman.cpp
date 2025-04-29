#include "jumpman.h"
//Based on the work of: Jordi Santiago

#include "input.h"
#include "assets.h"
#include "anim_data.h"
#include "debug.h"
#include "cistell.h"
#include "plant.h"
#include "stats_tracker.h"

extern float mainClock;

// accel
const float run_acc = 1400;
const float run_acc_onair = 400;
const float gravity_acc = 600;

// friction X
const float fri_acc_floor = 1000;
const float fri_acc_floor_crouched = 450;
const float fri_acc_air = 145;

// friction Y
const float fri_acc_wall_up = 1200;
const float fri_acc_wall_down = 450;

// jump
const float vel_jump = -150;
const float vel_walljump = 90;
const float jump_time = 0.35f;
const float timeCrouchedToJumpDownOneWayTile = 0.2f;

// limits
const vec vel_max(220, 200);
const vec vel_max_plant(110, 80);
const vec vel_max_water(150, 140);
const vec vel_max_tomato(150, 140);

// damage
const vec vel_hit(180.f, -150.f);
const float invencibleTimeAfterHit = 0.5f;

// Sprite
const vec standing_size = vec(16, 32);
const vec crouched_size = vec(16, 22);
const vec center = vec(8, 16);


inline vec posOnRightSlope(vec pos) {
	vec rounded = (TileMap::toTiles(pos) * Tile::size);
	vec diff = pos - rounded;
	float targetY = (Tile::size - diff.x);
	if (targetY < diff.y) {
		pos.y = rounded.y + targetY;
	}
	return pos;
}

inline vec posOnLeftSlope(vec pos) {
	vec rounded = (TileMap::toTiles(pos) * Tile::size);
	vec diff = pos - rounded;
	float targetY = diff.x;
	if (targetY < diff.y) {
		pos.y = rounded.y + targetY;
	}
	return pos;
}

JumpMan::JumpMan(int id)
	: id(id)
	, polvito(Assets::hospitalTexture)
	, animation(MARIO_IDLE)
{
	polvito.AddSprite({ 69, 50, 2, 2 });

	size = standing_size;
	InitPolvito();
}

void JumpMan::Update(float dt)
{
	if (dead) return;

	TileMap* map = TileMap::instance();

	const float marginGrounded = 1.5f; //in pixels
	grounded = false;
	Tile t;
	vec leftFoot(pos.x - center.x + 1.f, pos.y + marginGrounded);
	vec rightFoot(pos.x + center.x - 1.f, pos.y + marginGrounded);
	vec middleFoot(pos.x, pos.y + marginGrounded);
	t = map->getTile(TileMap::toTiles(leftFoot));
	if (t.isFullSolid()) {
		grounded = true;
		goto grounded_exit;
	} 
	if (t.isOneWay() && TileMap::alignToTiles(leftFoot.y) + 1.f > pos.y) {
		grounded = true;
		goto grounded_exit;
	}
	t = map->getTile(TileMap::toTiles(rightFoot));
	if (t.isFullSolid()) {
		grounded = true;
		goto grounded_exit;
	}
	if (t.isOneWay() && TileMap::alignToTiles(rightFoot.y) + 1.f > pos.y) {
		grounded = true;
		goto grounded_exit;
	}
	if (map->isPosOnSlope(middleFoot)) {
		grounded = true;
		goto grounded_exit;
	}
grounded_exit:

	crouched = ((crouched || grounded) && Input::IsPressed(id, GameKeys::DOWN)) || (crouched && !grounded);
	if (crouched) {
		crouchedTime += dt;
	}
	else {
		crouchedTime = 0.f;
	}

	if (Input::IsJustPressed(id, GameKeys::UP, 0.15f) && (grounded || (onWall && !crouched)))
	{
		//if (!Input::IsJustPressed(id, GameKeys::UP)) Debug::out << "cheats";
		Input::ConsumeJustPressed(id, GameKeys::UP);

		jumpTimeLeft = jump_time; // the jump upwards velocity can last up to this duration
		if (onWall && !grounded && !crouched) {
			vel.x = vel_walljump * -1.0f * float(onWall);
			lookingLeft = !lookingLeft;
			if (Rand::OnceEvery(2)) {
				Assets::soundJump1.Play();
			}
			else {
				Assets::soundJump2.Play();
			}
			StatsTracker::jumps_done += 1;
			StatsTracker::walljumps_done += 1;
			DoPolvitoWallJump();
		}
		else {
			bool ceiling = map->isSolid(TileMap::toTiles(pos.x - center.x + 1.f, pos.y - size.y - 1.f)) || map->isSolid(TileMap::toTiles(pos.x + center.x - 1.f, pos.y - size.y - 1.f));
			if (!ceiling) {
				if (Rand::OnceEvery(2)) {
					Assets::soundJump1.Play();
				}
				else {
					Assets::soundJump2.Play();
				}
				StatsTracker::jumps_done += 1;
				DoPolvitoJump();
				grounded = false;
			}
		}
	}

	vec acc = vec(0, 0);
	if (Input::IsPressed(id, GameKeys::LEFT)) {
		lookingLeft = true;
		if (grounded) {
			if (!crouched) acc.x -= run_acc;
		}
		else {
			acc.x -= run_acc_onair;
		}
	}
	if (Input::IsPressed(id, GameKeys::RIGHT)) {
		lookingLeft = false;
		if (grounded) {
			if (!crouched) acc.x += run_acc;
		}
		else {
			acc.x += run_acc_onair;
		}
	}

	if (Input::IsPressed(id, GameKeys::UP) && jumpTimeLeft > 0)
	{
		vel.y = vel_jump;
	}
	else
	{
		jumpTimeLeft = 0; // Stop jumping
	}

	if (jumpTimeLeft > 0)
	{
		jumpTimeLeft -= dt; //We are still jumping
	}
	else
	{
		acc.y += gravity_acc;
	}

	//FRICTION
	vec fri = vec(0, 0);
	if (grounded)
	{
		if (crouched)
		{
			fri.x = fri_acc_floor_crouched;
		}
		else
		{
			fri.x = fri_acc_floor;
		}
	}
	else
	{
		fri.x = fri_acc_air;
	}

	if (!crouched && onWall)
	{
		if (vel.y < 0) { // Moving up
			fri.y = fri_acc_wall_up;
		}
		else {
			fri.y = fri_acc_wall_down;
		}
	}

	// Apply friction
	if (vel.x < 0)
	{
		vel.x += fri.x * dt;
		if (vel.x > 0) vel.x = 0;
	}
	else if (vel.x > 0)
	{
		vel.x -= fri.x * dt;
		if (vel.x < 0) vel.x = 0;
	}
	if (fri.y != 0)
	{
		if (vel.y < 0)
		{
			vel.y += fri.y * dt;
			if (vel.y > 0) vel.y = 0;
		}
		else if (vel.y > 0)
		{
			vel.y -= fri.y * dt;
			if (vel.y < 0) vel.y = 0;
		}

	}

	vel = vel + acc * dt;

	//Clamp vel
	vec limit = vel_max;
	if (holding == Holdable::Plant) limit = vel_max_plant;
	if (holding == Holdable::Basket) {
		if (cistell->contents == Cistell::WATER) limit = vel_max_water;
		if (cistell->contents == Cistell::TOMATOES) limit = vel_max_tomato;
	}
	if (vel.x > limit.x) vel.x = limit.x;
	if (vel.x < -limit.x) vel.x = -limit.x;
	if (vel.y < -limit.y) vel.y = -limit.y;
	if (vel.y > vel_max.y) vel.y = vel_max.y;

	vec appliedVel = vel;
	Tile tileAtMyFeet = map->getTile(TileMap::toTiles(pos.x, pos.y - 0.1f));
	if (tileAtMyFeet.isSlope()) {
		// On a slope, we will override the Y displacement with the X displacement, either upwards or downwards.
		// Pythagoras wouldn't approve that we move at the same velocity on a flat X axis than when move on both X and Y.
		// The mathematically accurate value would be /2 but we don't want to slow the player that much either.
		appliedVel.x = sqrt((vel.x * vel.x) / 1.8f);
		if (vel.x < 0) appliedVel.x = -appliedVel.x;
	}
	vec posf = pos + appliedVel * dt;

	//Debug::out << "----------";
	//Debug::out << abs(pos.x - posf.x) / dt;
	//Debug::out << "slope=" << tileAtMyFeet.isSlope();

	vec direction = posf - pos;
	const float E = 1;
	vec centerFromRight = size - center;

	//Skip horizontal collisions when on a slope
	if (tileAtMyFeet.isSlope()) {
		goto horz_exit;
	}

	if (direction.x < 0) //Moving left
	{

		int xo = map->toTiles(pos.x - center.x);
		int xn = map->toTiles(posf.x - center.x);
		int yTop = map->toTiles(pos.y - size.y + E);
		int yBottom = map->toTiles(pos.y - E);
		for (int x = xo; x >= xn; x--)
		{
			for (int y = yTop; y <= yBottom; y++)
			{
				Tile t = map->getTile(x, y);
				if (t.isFullSolid())
				{
					posf.x = map->Right(x) + center.x;
					vel.x = -500.f * dt; //stay against wall
					if (!isHit()) {
						onWall = ONWALL_LEFT;
					}
					lookingLeft = true;
					goto horz_exit;
				}
			}
		}
		//No collision left
	}
	else if (direction.x > 0) //Moving right
	{
		int xo = map->toTiles(pos.x + centerFromRight.x);
		int xn = map->toTiles(posf.x + centerFromRight.x);
		int yTop = map->toTiles(pos.y - size.y + E);
		int yBottom = map->toTiles(pos.y - E);
		for (int x = xo; x <= xn; x++)
		{
			for (int y = yTop; y <= yBottom; y++)
			{
				Tile t = map->getTile(x, y);
				if (t.isFullSolid())
				{
					posf.x = map->Left(x) - centerFromRight.x;
					vel.x = 500.f * dt; //stay against wall
					if (!isHit()) {
						onWall = ONWALL_RIGHT;
					}
					lookingLeft = false;
					goto horz_exit;
				}
			}
		}
		//No collision right
	}
	onWall = ONWALL_NO;

horz_exit:
	pos.x = posf.x;

	if (direction.y < 0) //Moving up
	{
		int yo = map->toTiles(pos.y - size.y); // top edge
		int yn = map->toTiles(posf.y - size.y);
		int xl = map->toTiles(pos.x - center.x + E);
		int xr = map->toTiles(pos.x + centerFromRight.x - E);
		for (int y = yo; y >= yn; y--)
		{
			for (int x = xl; x <= xr; x++)
			{
				Tile t = map->getTile(x, y);
				if (t.isSolid()) //slopes should be collisionable when going up
				{
					posf.y = map->Bottom(y) + size.y;
					vel.y = 0;
					jumpTimeLeft = 0;
					goto vert_exit;
				}
			}
		}
		//No collision up
	}
	else if (direction.y > 0) //Moving down
	{
		float max_movement_into_slope = abs(appliedVel.x * dt) + 1.f;

		//Debug::out << "getTile from=" << TileMap::offsetInTile(posf.x, pos.y - max_movement_into_slope) << ": " << map->getTile(TileMap::toTiles(vec(posf.x, pos.y - max_movement_into_slope)));
		//Debug::out << "getTile to=" << TileMap::offsetInTile(posf.x, pos.y + max_movement_into_slope) << ": " << map->getTile(TileMap::toTiles(vec(posf.x, pos.y + max_movement_into_slope)));

		for (int y = floor(pos.y - max_movement_into_slope); y < ceil(pos.y + max_movement_into_slope); y++) {
			if (map->isPosOnSlope(posf.x, y-0.00001f)) { // hack: we want to get to the edge of a tile before stepping onto the next one, hence the epsilon deduced from the integer value.
				//Debug::out << "set y on slope";
				posf.y = y;
				if (vel.y > 50) {
					Assets::soundLand.Play();
					DoPolvitoLand();
				}
				vel.y = 30.f; //this helps you get grounded as soon as the slope ends
				onWall = ONWALL_NO;
				grounded = true;
				goto vert_exit;
			}
		}

		int yo = map->toTiles(pos.y); // bottom edge
		int yn = map->toTiles(posf.y);
		int xl = map->toTiles(pos.x - center.x + E);
		int xr = map->toTiles(pos.x + centerFromRight.x - E);
		for (int y = yo; y <= yn; y++)
		{
			for (int x = xl; x <= xr; x++)
			{
				Tile t = map->getTile(x, y);
				if ((t.isFullSolid()) || (t.isOneWay() && pos.y-1.f < (y*Tile::size)))
				{
					if (t.isOneWay() && crouchedTime > timeCrouchedToJumpDownOneWayTile) {
						posf.y = map->Top(y)+3.f;
						vel.y = 0;
						crouched = false;
						grounded = false;
						goto vert_exit;
					} else {
						//Debug::out << "terra";
						posf.y = map->Top(y);
						if (vel.y > 50) DoPolvitoLand();
						vel.y = 0;
						onWall = ONWALL_NO;
						grounded = true;
						goto vert_exit;
					}
				}
			}
		}
		//No collision down
	}
vert_exit:
	pos = posf;

	animation.Update((int)(dt * 1000));

	bool isWalking = false;
	bool isTurning = false;
	if (crouched)
	{
		size = crouched_size;
		animation.Ensure(MARIO_CROUCH);
	}
	else
	{

		size = standing_size;
		if (grounded)
		{
      if (Input::IsPressed(id, GameKeys::LEFT) && !Input::IsPressed(id, GameKeys::RIGHT))
      {
        isWalking = true;
        if (vel.x > 0) {
          animation.Ensure(MARIO_TURN);
          isTurning = true;
        }
        else {
          animation.Ensure(MARIO_WALK);
        }
      }
      else if (Input::IsPressed(id, GameKeys::RIGHT) && !Input::IsPressed(id, GameKeys::LEFT))
			{
				isWalking = true;
				if (vel.x < 0) {
					animation.Ensure(MARIO_TURN);
					isTurning = true;
				}
				else animation.Ensure(MARIO_WALK);
			}
			else
			{
				animation.Ensure(MARIO_IDLE);
			}
		}
		else
		{
			if (onWall) animation.Ensure(MARIO_ONWALL);
			else animation.Ensure(MARIO_JUMP);
		}
	}
	if (isWalking) {
		DoPolvitoRun(dt, (acc.x < 0), isTurning);
	}
	polvito.UpdateParticles(dt);

	if (invencibleTimer > 0.f) {
		invencibleTimer -= dt;
	}
}

void JumpMan::takeDamage(const vec& src) {
	invencibleTimer = invencibleTimeAfterHit;
	if (pos.x > src.x) {
		vel.x = vel_hit.x;
	}
	else {
		vel.x = -vel_hit.x;
	}
	if (grounded) {
		vel.y = vel_hit.y;
	}
	jumpTimeLeft = 0;
	onWall = ONWALL_NO;
	crouched = false;
}

BoxBounds JumpMan::maxBounds() const
{
	return BoxBounds(pos, standing_size, vec(standing_size.x / 2, standing_size.y));
}

void JumpMan::Draw() const {

	polvito.Draw();

	//Assets::tintShader.Activate();
	//Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);

	auto rect = animation.CurrentFrameRect();
	rect.y += 32*id;
	Window::Draw(Assets::marioTexture, pos)
		.withScale(lookingLeft? -1.25f : 1.25f, 1.25f)
		.withOrigin(center.x, size.y)
		.withRect(rect);

	//Assets::tintShader.Deactivate();
}



// BRILLI-BRILLI

void JumpMan::InitPolvito() {
	polvito.min_interval = 0.01f;
	polvito.max_interval = 0.05f;

	polvito.min_ttl = 0.1f;
	polvito.max_ttl = 0.3f;

	polvito.min_vel.x = 3;
	polvito.max_vel.x = 18;

	polvito.min_vel.y = -50.f;
	polvito.max_vel.y = -20.f;

	polvito.acc.y = 120.f;

	polvito.min_scale = 1.0f;
	polvito.max_scale = 1.8f;
	polvito.scale_vel = -3.5f;

	polvito.min_rotation = 0.f;
	polvito.max_rotation = 360.f;
	polvito.min_rotation_vel = 160.f;
	polvito.max_rotation_vel = 160.f;

	polvito.alpha_vel = -1.8f;
}

inline void JumpMan::DoPolvitoJump() {
	// Pluf cap als dos costats
	polvito.pos = pos + vec(-1.5f, -1.5f);
	if (polvito.min_vel.x > 0) {
		polvito.FlipX();
	}
	polvito.AddParticles(2);
	polvito.pos = pos + vec(1.5f, -1.5f);
	polvito.FlipX();
	polvito.AddParticles(2);
}

inline void JumpMan::DoPolvitoWallJump() {
	if (vel.x > 0) {
		polvito.pos = pos + vec(-7.f, -16.f);
	}
	else {
		polvito.pos = pos + vec(7.f, -16.f);
	}

	if ((vel.x < 0 && polvito.min_vel.x > 0) || (vel.x > 0 && polvito.min_vel.x < 0)) {
		polvito.FlipX();
	}
	polvito.AddParticles(5);
}

inline void JumpMan::DoPolvitoLand() {
	// Pluf cap als dos costats
	polvito.pos = pos + vec(-8.f, -0.3f);
	if (polvito.min_vel.x > 0) {
		polvito.FlipX();
	}
	polvito.AddParticles(3);
	polvito.pos = pos + vec(8.f, -0.3f);
	polvito.FlipX();
	polvito.AddParticles(3);
}

inline void JumpMan::DoPolvitoRun(float dt, bool toTheLeft, bool doTheExtraPolvitoLikeYouKnowItsDone) {
	if (toTheLeft) {
		polvito.pos = pos + vec(4.f, -0.5f);
		if (polvito.min_vel.x < 0) {
			polvito.FlipX();
		}
	}
	else {
		polvito.pos = pos + vec(-4.f, -0.5f);
		if (polvito.min_vel.x > 0) {
			polvito.FlipX();
		}
	}
	if (doTheExtraPolvitoLikeYouKnowItsDone) {
		polvito.AddParticles(2);
	}
	polvito.Spawn(dt);
}

void JumpMan::Carry(Plant* plant)
{
	holding = Holdable::Plant;
	this->plant = plant;
	plant->PickUpBy(this);
	++StatsTracker::plant_pickedup;
}

void JumpMan::Carry(Cistell* cistell)
{
	holding = Holdable::Basket;
	this->cistell = cistell;
	cistell->PickUpBy(this);
	++StatsTracker::basked_pickedup;
}

void JumpMan::DropItem()
{
	assert(holding != Holdable::None);
	if (holding == Holdable::Plant) {
		plant->Drop();
		plant = nullptr;
	} else {
		cistell->Drop();
		cistell = nullptr;
	}
	holding = Holdable::None;
}
