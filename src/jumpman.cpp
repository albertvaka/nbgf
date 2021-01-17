#include "jumpman.h"

#include "input.h"
#include "bullet.h"
#include "mates.h"
#include "assets.h"
#include "fxmanager.h"
#include "oneshotanim.h"
#include "debug.h"
#include "savestate.h"
#include "common_tilemapcharacter.h"
#include "skilltree.h"

extern float mainClock;

// accel
const float run_acc = 1400;
const float run_acc_onair = 400;
const float gravity_acc = 660;

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
const vec vel_max(220, 350);

// bfg
const float bulletVel = 400.f;
const float bfgCooldown = 0.6f;
const float bfgPushBack = 150.f;

// damage
const vec vel_hit(180.f, -150.f);
const float invencibleTimeAfterHit = 0.5f;

// Sprite
const vec standing_size = vec(16, 32);
const vec crouched_size = vec(16, 22);
const vec center = vec(8, 16);

JumpMan::JumpMan()
	: polvito(Assets::hospitalTexture)
	, animation(MARIO_IDLE)
	, size(standing_size)
{
	InitPolvito();
}

void JumpMan::SaveGame(SaveState& state) const {
	state.StreamPut("player") << pos.x << pos.y << maxHealth;
}

void JumpMan::LoadGame(const SaveState& state) {
	state.StreamGet("player") >> pos.x >> pos.y >> maxHealth;
	Reset(pos, maxHealth);
}

void JumpMan::Update(float dt)
{
	if (frozen || !alive) return;

	TileMap* map = TileMap::instance();

	grounded = IsGrounded(pos - vec(0,size.y/2), size);

	crouched = ((crouched || grounded) && Input::IsPressed(0,GameKeys::CROUCH)) || (crouched && !grounded);
	if (crouched) {
		crouchedTime += dt;
	}
	else {
		crouchedTime = 0.f;
	}

	if (Input::IsJustPressed(0,GameKeys::JUMP, 0.15f) && (grounded || (onWall && !crouched)))
	{
		//if (!Input::IsJustPressed(0,GameKeys::JUMP)) Debug::out << "cheats";
		Input::ConsumeJustPressed(0, GameKeys::JUMP);

		jumpTimeLeft = jump_time; // the jump upwards velocity can last up to this duration
		if (onWall && !grounded && !crouched) {
			vel.x = vel_walljump * -1.0f * float(onWall);
			lookingLeft = !lookingLeft;
			DoPolvitoWallJump();
		}
		else {
			bool ceiling = map->getTile(TileMap::toTiles(pos.x - center.x + 1.f, pos.y - size.y - 1.f)).isSolid() || map->getTile(TileMap::toTiles(pos.x + center.x - 1.f, pos.y - size.y - 1.f)).isSolid();
			if (!ceiling) {
				DoPolvitoJump();
				grounded = false;
			}
		}
	}

	vec acc = vec(0, 0);
	if (Input::IsPressed(0,GameKeys::LEFT)) {
		lookingLeft = true;
		if (grounded) {
			if (!crouched) acc.x -= run_acc;
		}
		else {
			acc.x -= run_acc_onair;
		}
	}
	if (Input::IsPressed(0,GameKeys::RIGHT)) {
		lookingLeft = false;
		if (grounded) {
			if (!crouched) acc.x += run_acc;
		}
		else {
			acc.x += run_acc_onair;
		}
	}

	if (Input::IsPressed(0,GameKeys::JUMP) && jumpTimeLeft > 0)
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
	if (vel.x > vel_max.x) vel.x = vel_max.x;
	if (vel.x < -vel_max.x) vel.x = -vel_max.x;
	if (vel.y > vel_max.y) vel.y = vel_max.y;
	if (vel.y < -vel_max.y) vel.y = -vel_max.y;

	
	//Do move
	MoveResult moved = MoveAgainstTileMap(pos - vec(0, size.y/2), size, vel, dt);
	pos = moved.pos + vec(0, size.y/2);

	if (moved.leftWallCollision != Tile::NONE) {
		if (!isHit() && SkillTree::instance()->IsEnabled(Skill::WALLJUMP)) {
			onWall = ONWALL_LEFT;
			vel.x = -500.f * dt; //stay against wall
		}
		else {
			vel.x = 0;
		}
		lookingLeft = true;
	}
	else if (moved.rightWallCollision != Tile::NONE) {
		if (!isHit() && SkillTree::instance()->IsEnabled(Skill::WALLJUMP)) {
			onWall = ONWALL_RIGHT;
			vel.x = 500.f * dt; //stay against wall
		}
		else {
			vel.x = 0;
		}
		lookingLeft = false;
	}
	else {
		onWall = ONWALL_NO;
	}

	if (moved.ceilingCollision != Tile::NONE) {
		vel.y = 0;
		jumpTimeLeft = 0;
	}
	if (moved.groundCollision != Tile::NONE) {
		if (moved.groundCollision.isOneWay() && crouchedTime > timeCrouchedToJumpDownOneWayTile) {
			pos.y += 3.f;
			vel.y = 0;
			crouched = false;
			grounded = false;
		}
		else {
			if (vel.y > 50) DoPolvitoLand();
			if (moved.groundCollision.isSlope()) {
				vel.y = 30.f; //this helps you get grounded as soon as the slope ends
			}
			else {
				vel.y = 0;
			}
			onWall = ONWALL_NO;
			grounded = true;
		}
	}




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
			if (Input::IsPressed(0,GameKeys::LEFT) && !Input::IsPressed(0,GameKeys::RIGHT))
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
			else if (Input::IsPressed(0,GameKeys::RIGHT) && !Input::IsPressed(0,GameKeys::LEFT))
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


	if (SkillTree::instance()->IsEnabled(Skill::GUN)) {
		bfgPos = pos + bfgOffset();
		bfgAngle = bfgPos.AngleDegs(Mouse::GetPositionInWorld());
		bfgAngle = (int(bfgAngle + 360 + (45.f / 2)) / 45) * 45.f;
		if (bfgCooldownTimer > 0.f) {
			bfgCooldownTimer -= dt;
			if (bfgCooldownTimer < 0.f) {
				bfgCooldownTimer = 0.f;
			}
		}
		else if (Mouse::IsPressed() && !(Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT))) {
			bfgCooldownTimer = bfgCooldown;
			vec gunDirection = vec::FromAngleDegs(bfgAngle);
			vec tipOfTheGun = bfgPos + gunDirection*17.f;
			new Bullet(tipOfTheGun, gunDirection*bulletVel, 1.5f);
			vel -= gunDirection*bfgPushBack;
			jumpTimeLeft = 0; // Overrides jump impulse
			if (onWall) {
				vel.x = 0; // Will let wall go if we shoot and we aren't explicitly moving towards the wall
			}
			if (grounded) {
				if (abs(vel.x) < 0.1) {
					DoPolvitoLand();
				}
				else {
					DoPolvitoRun(dt, vel.x < 0, true);
					DoPolvitoRun(dt, vel.x < 0, true);
				}
			}
		}
	}

	if (invencibleTimer > 0.f) {
		invencibleTimer -= dt;
	}
}

void JumpMan::takeDamage(vec src) {
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
	FxManager::StopTheWorld(0, 0.25f);
	vec playerCenter = bounds().Center();
	float direction = (playerCenter-src).AngleDegs();
	new OneShotAnim(Assets::marioTexture, playerCenter, AnimLib::HIT_SPLASH, 2, direction);
	health--;
}

Bounds JumpMan::maxBounds() const
{
	return Bounds(pos, standing_size, vec(standing_size.x / 2, standing_size.y));
}

void JumpMan::Draw() const {

	polvito.Draw();

	if (isHit()) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
	}

	Window::Draw(Assets::marioTexture, pos)
		.withOrigin(center.x, size.y)
		.withRect(animation.CurrentFrame())
		.withScale(lookingLeft ? -1.f : 1.f, 1.f);

	//BFG
	if (SkillTree::instance()->IsEnabled(Skill::GUN)) {
		GPU_Rect rect;
		if (bfgCooldownTimer > (bfgCooldown - bfgCooldown / 4.f)) {
			bool blink = (int(mainClock * 100) % 2);
			rect = { blink ? 32 * 3.f : 32 * 2.f, 3 * 16.f, 2 * 16.f, 16.f };
		}
		else {
			bool shake = (int(mainClock * 6) % 10) > 8;
			rect = { shake ? 32 : 0.f, 3 * 16.f, 2 * 16.f, 16.f };
		}
		float scale = (0.333f + (std::max(bfgCooldown / 1.5f, bfgCooldownTimer) / bfgCooldown));
		vec vscale = vec(scale, scale);
		if (bfgAngle < 270 || bfgAngle  > 450) {
			vscale.y = -vscale.y;
		}
		Window::Draw(Assets::marioTexture, bfgPos)
			.withRect(rect)
			.withOrigin(10, 8)
			.withRotationDegs(bfgAngle)
			.withScale(vscale);
	}

	Shader::Deactivate();

	if (Debug::Draw) {
		bounds().Draw();
		//player.pos.Debuggerino(sf::Color::White);
		//player.bounds().Center().Debuggerino(sf::Color::Magenta);
	}

}

void JumpMan::DrawGUI() const {
	for (int i = 0; i < maxHealth; i++) {
		Window::Draw(Assets::marioTexture, 10 + 16 * i, 10)
			.withRect(i < health ? AnimLib::HEALTH_FULL : AnimLib::HEALTH_EMPTY);
	}
}

// BRILLI-BRILLI

void JumpMan::InitPolvito() {
	polvito.AddSprite({ 69.f, 50.f, 2.f, 2.f });

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
	polvito.rotation_vel = 160.f;

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
