#include "player.h"

#include "input.h"
#include "bullet.h"
#include "mates.h"
#include "destroyedtiles.h"
#include "assets.h"
#include "assets_sounds.h"
#include "anim_lib.h"
#include "fx.h"
#include "oneshotanim.h"
#include "debug.h"
#include "savestate.h"
#include "particles.h"
#include "common_tilemapcharacter.h"
#include "common_bullet.h"
#include "skilltree.h"
#ifdef _IMGUI
#include "imgui.h"
#endif

extern float mainClock;

// input
const float kIsJustPressedIntervalTime = 0.15f;
const float kJumpTimeAfterBeingGrounded = 0.07f; //~4 frames

// accel
const float kRunAcc = 1400;
const float kRunAcc_OnAir = 1000;
const float kGravityAcc = 660;
const float kDiveHorizontalAcc = 10;

// friction X
const float kFrictAccFloor = 1000;
const float kFrictAccFloor_Crouched = 450;
const float kFrictAcc_OnAir = 500;

// friction Y
const float kFrictAccVert_WallUp = 400;
const float kFrictAccVert_WallDown = 540;

// jump
const float kVelJump = -200; // Y axis
const float kVelWalljump = 140; // X axis
const float kVelSlopejump = 140; // X axis
const float kJumpHeightDiff = 72.f; // vertical pixels (note: this is the height at wich we stop adding vertical velocity, but we jump slightly higher thant his, ~8px more;
const float kTimeCrouchedToJumpDownOneWayTile = 0.2f;
const float kTimeToJumpFromWallAfterLettingGo = 0.2f;

// dash
const float kVelDash = 400;
const float kDashCooldown = 1.5f;
const float kDashDuration = 0.22f;

// dive
const float kVelDive = 400;
const float kDiveRestTime = 0.4f; // time you can't move after touching ground
const float kDiveRestTimeAfterHitting = 0.3f;

// limits
const vec kVelMax(220, 350);

// bfg
const float kBulletVel = 400.f;
const float kBfgCooldown = 0.2f;
const float kBfgPushBack = 150.f;

// knockback
const float kDoDamageKnockbackVel = 100.f;
const float kDoDamageKnockbackVelGrounded = 140.f;
const float kDoDamageUpKnockbackVel = 180.f;
const float kDoDamageDownKnockbackVel = -220.f;
const vec kTakeDamageKnockbackVel(180.f, -150.f);
const float kInvencibleTimeAfterHit = 1.0f;

// sword attack
const float kSwordAttackRadius = 23.3f;
const vec kSwordAttackOffset = vec(16.f,-17.f);

const float kSwordAttackDownRadius = 13.f;
const vec kSwordAttackDownOffset = vec(2.f,3.f);

const float kSwordAttackUpRadius = 21.3f;
const vec kSwordAttackUpOffset = vec(0.f, -30.f);

const float kSwordAttackWallSlideRadius = 23.f;
const vec kSwordAttackWallSlideOffset = vec(-16.f, -15.5f);

// Sprite
const vec kStandingSize = vec(13, 32);
const vec kCrouchedSize = vec(13, 22);
const int kOffsetSwordlessSpritesheet = 413;

const float kHealthAnimationTime = 0.3f;

void DestroyTilesWithSword(const CircleBounds& e) {
	GaemTileMap* map = GaemTileMap::instance();
	DestroyedTiles* destroyedTiles = DestroyedTiles::instance();
	auto breakPower = SkillTree::instance()->GetBreakPower();
	int xLeft = e.pos.x - e.radius;
	int xRight = e.pos.x + e.radius;
	int yTop = e.pos.y - e.radius;
	int yBottom = e.pos.y + e.radius;
	for (int x = xLeft; x <= xRight; x+=Tile::Size)
	{
		for (int y = yTop; y <= yBottom; y+=Tile::Size) {
			vec pos = Tile::AlignToTiles(vec(x, y)) + Tile::Sizes/2; // IMPROVE?: here we only check the center of the tile
			if (e.Contains(pos)) {
				veci t = Tile::ToTiles(pos);
				Tile tile = map->GetTile(t);
				if (tile.isBreakable(breakPower)) {
					destroyedTiles->Destroy(t.x, t.y);
				}
			}
		}
	}
}

Player::Player()
	: anim(AnimLib::WARRIOR_IDLE)
	, playerAttack(vec::Zero, kSwordAttackRadius)
	, size(kStandingSize)
	, lastSafeTilePos(-1,-1)
{
}

void Player::SaveGame(SaveState& state) const {
	state.StreamPut("player") << pos.x << pos.y << maxHealth;
}

void Player::LoadGame(const SaveState& state) {
	state.StreamGet("player") >> pos.x >> pos.y >> maxHealth;
	Reset(pos, maxHealth);
}

void Player::DealDamage(vec target) {
	if (diving) {
		vel.y = kDoDamageDownKnockbackVel;
		divingRestTimer = kDiveRestTimeAfterHitting;
	}
	else if (attackingUp) {
		vel.y += kDoDamageUpKnockbackVel;
	} else {
		// Note: This gets called multiple times when the enemy doesn't die on a single frame, so you get a bigger knockback
		float knockback = groundTile != Tile::NONE ? kDoDamageKnockbackVelGrounded : kDoDamageKnockbackVel;
		if (pos.x < target.x) {
			knockback = -knockback;
		}
		float newVelX = vel.x / 2 + knockback;
		//if (groundTile == Tile::NONE || !IsGoingToRunOffPlatform(CenterPos(), size, vec(newVelX, vel.y), 1 / 60.f))
		vel.x = newVelX;
	}
	initialJumpY = Mates::MaxFloat;
	onWall = false;
	crouched = false;
	dashing = false;
	diving = false;
}

void Player::Reset(vec position, int maxHp) {
	pos = position;
	bfgPos = position + bfgOffset();
	vel = vec(0.f, 0.f);
	lastSafeTilePos = Tile::ToTiles(position);
	invencibleTimer = -1.f;
	bfgCooldownTimer = 0.f;
	healthAnimationOldHealth = health = maxHealth = maxHp;
	healthAnimationTimer = 0.f;
	initialJumpY = Mates::MaxFloat;
	crouchedTime = 0.0f;
	timeAfterBeingGrounded = Mates::MaxFloat;
	onWall = false;
	frozen = false;
	alive = true;
	groundTile = Tile::NONE;
	canDash = true;
	diving = false;
	dashing = false;
	crouched = false;
	lookingLeft = false;
	dashTimer = 0.f;
	dashCooldown = 0.f;
	divingRestTimer = 0.f;
	justHit = false;
	jumpFromWallTimer = 0.f;
	attacking = false;
	attackingUp = false;
	playerAttack.alive = false;
	ScreenManager::UpdateCurrentScreen(pos);
}

void Player::UpdateMoving(float dt) 
{
	crouched = (groundTile != Tile::NONE && Input::IsPressed(0, GameKeys::CROUCH));
	if (crouched) {
		crouchedTime += dt;
	}
	else {
		crouchedTime = 0.f;
	}

	vec acc = vec(0, 0);
	if (Input::IsPressed(0,GameKeys::LEFT)) {
		if (!attacking) lookingLeft = true;
		if (groundTile != Tile::NONE) {
			if (!crouched) acc.x -= kRunAcc;
		}
		else {
			acc.x -= kRunAcc_OnAir;
		}
	}
	if (Input::IsPressed(0,GameKeys::RIGHT)) {
		if (!attacking) lookingLeft = false;
		if (groundTile != Tile::NONE) {
			if (!crouched) acc.x += kRunAcc;
		}
		else {
			acc.x += kRunAcc_OnAir;
		}
	}

	if (pos.y > initialJumpY - kJumpHeightDiff && Input::IsPressed(0, GameKeys::JUMP))
	{
		vel.y = kVelJump;
	}
	else
	{
		initialJumpY = Mates::MaxFloat;
		acc.y += kGravityAcc;
	}

	// Calculate friction
	vec fri = vec(0, 0);
	if (groundTile != Tile::NONE)
	{
		if (crouched)
		{
			fri.x = kFrictAccFloor_Crouched;
		}
		else
		{
			fri.x = kFrictAccFloor;
		}
	}
	else
	{
		fri.x = kFrictAcc_OnAir;
	}

	if (!crouched && onWall)
	{
		if (vel.y < 0) { // Moving up
			fri.y = kFrictAccVert_WallUp;
		}
		else {
			fri.y = kFrictAccVert_WallDown;
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

}

#ifdef _IMGUI
float debugMaxJumpY = 0;
#endif

void Player::Update(float dt)
{
	if (frozen || !alive) return;

	if (voiceSoundChannel != -1 && !Sound::Playing(voiceSoundChannel)) {
		voiceSoundChannel = -1;
	}

	justHit = false;

	int prev_frame = anim.current_frame;
	if (anim.IsSet(AnimLib::WARRIOR_WALL_SLIDE)) {
		if (vel.y < 10.f) {
			// do not update animation
			anim.current_frame = 2;
		} else if (vel.y < 90.f) {
			anim.Update(dt * (vel.y / 100.f));
			if (anim.current_frame == 1) {
				// hack: skip frame that looks weird if slowed down
				anim.current_frame = 2;
			}
		} else {
			anim.Update(dt);
		}
	}
	else {
		anim.Update(dt);
	}
	bool animationNewFrame = (prev_frame != anim.current_frame);


	if (groundTile != Tile::NONE || onWall) {
		canDash = true;
	}

	if (groundTile != Tile::NONE) {
		onWall = false;
	}

	dashCooldown -= dt;
	if (!diving && SkillTree::instance()->IsEnabled(Skill::DASH) && canDash && dashCooldown <= 0.f) {
		if (Input::IsJustPressed(0, GameKeys::DASH, kIsJustPressedIntervalTime)) {
			Input::ConsumeJustPressed(0, GameKeys::DASH);
			Sound::Stop(voiceSoundChannel);
			voiceSoundChannel = Assets::soundVoiceJump.Play();
			dashCooldown = kDashCooldown;
			dashTimer = 0.f;
			dashing = true;
			canDash = false;
			if (onWall) {
				lookingLeft = !lookingLeft;
			}
			initialJumpY = Mates::MaxFloat;
			vel = vec(lookingLeft? -kVelDash : kVelDash, 0);
			attacking = false;
			anim.Ensure(AnimLib::WARRIOR_DASH);
		}
	}

	if (!dashing && !diving && SkillTree::instance()->IsEnabled(Skill::DIVE) && divingRestTimer <= 0.f) {
		if (groundTile == Tile::NONE && Input::IsPressed(0, GameKeys::CROUCH) && Input::IsJustPressed(0, GameKeys::ATTACK, kIsJustPressedIntervalTime)) {
			Input::ConsumeJustPressed(0, GameKeys::ATTACK);
			diving = true;
			Sound::Stop(voiceSoundChannel);
			voiceSoundChannel = Assets::soundVoiceDiveAttack.Play();
			attacking = false;
			anim.Ensure(AnimLib::WARRIOR_ATTACK_DOWN_TRANSITION, false);
		}
	}

	if (!dashing && !diving && !attacking && SkillTree::instance()->IsEnabled(Skill::ATTACK) && (!Input::IsPressed(0, GameKeys::CROUCH) || !SkillTree::instance()->IsEnabled(Skill::DIVE))) {
		if (Input::IsJustPressed(0, GameKeys::ATTACK, kIsJustPressedIntervalTime)) {
			Input::ConsumeJustPressed(0, GameKeys::ATTACK);
			attacking = true;
			Sound::Stop(voiceSoundChannel);
			voiceSoundChannel = Assets::soundVoiceAttack.Play();
			Assets::soundSword.Play();
			if (onWall) {
				attackingUp = false;
				anim.Ensure(AnimLib::WARRIOR_WALL_SLIDE_ATTACK, false);
			}
			else {
				attackingUp = Input::IsPressed(0, GameKeys::MENU_UP);
				if (attackingUp) {
					anim.Ensure(AnimLib::WARRIOR_ATTACK_UP, false);
				}
				else {
					anim.Ensure(AnimLib::WARRIOR_MOVING_ATTACK, false);
				}
			}
		}
	}

	if (dashing) {
		dashTimer += dt;
		if (dashTimer >= kDashDuration) {
			dashing = false;
		}
	}

	if (attacking) {
		playerAttack.alive = (anim.current_frame == 1 || anim.current_frame == 2);
		if (playerAttack.alive) {
			DestroyTilesWithSword(playerAttack.Bounds());
		}
		if (!attackingUp && anim.current_frame == 0 && Input::IsPressed(0, GameKeys::MENU_UP)) {
			attackingUp = true;
		}
		if (onWall) {
			anim.anim = AnimLib::WARRIOR_WALL_SLIDE_ATTACK.data();
			playerAttack.radius = kSwordAttackWallSlideRadius;
			playerAttack.pos = pos + kSwordAttackWallSlideOffset.Mirrored(lookingLeft, false);
		}
		else if (attackingUp) {
			anim.anim = AnimLib::WARRIOR_ATTACK_UP.data();
			playerAttack.radius = kSwordAttackUpRadius;
			playerAttack.pos = pos + kSwordAttackUpOffset.Mirrored(lookingLeft, false);
		}
		else {
			anim.anim = AnimLib::WARRIOR_MOVING_ATTACK.data();
			playerAttack.radius = kSwordAttackRadius;
			playerAttack.pos = pos + kSwordAttackOffset.Mirrored(lookingLeft, false);
		}

		if (anim.IsComplete()) {
			attacking = false;
		}
	} else if (diving) {
		if (divingRestTimer > 0.f) {
			divingRestTimer -= dt;
			playerAttack.alive = false;
			if (divingRestTimer <= 0.f) {
				diving = false;
			}
			if (anim.complete) { //standup anim done
				anim.Ensure(AnimLib::WARRIOR_IDLE);
			}
		} else if (anim.complete) {
			vel = vec(0, kVelDive);
			anim.Ensure(AnimLib::WARRIOR_ATTACK_DOWN);
			playerAttack.radius = kSwordAttackDownRadius;
			playerAttack.alive = true;
			//Debug::FrameByFrame = true;
		}
		playerAttack.pos = pos + kSwordAttackDownOffset.Mirrored(lookingLeft, false);
	} else {
		if (divingRestTimer > 0.f) {
			divingRestTimer -= dt;
		}
		playerAttack.alive = false;
	}

	if (onWall) {
		jumpFromWallTimer = kTimeToJumpFromWallAfterLettingGo;
	} else {
		jumpFromWallTimer -= dt;
	}

	if (groundTile != Tile::NONE) {
		timeAfterBeingGrounded = 0.f;
	}
	else {
		timeAfterBeingGrounded += dt;
	}

	if (!dashing && !diving) {

		GaemTileMap* map = GaemTileMap::instance();

		// JUMPERINO
		if (Input::IsJustPressed(0, GameKeys::JUMP, kIsJustPressedIntervalTime) && (timeAfterBeingGrounded < kJumpTimeAfterBeingGrounded || jumpFromWallTimer > 0.f)) {
			Input::ConsumeJustPressed(0, GameKeys::JUMP);

#ifdef _IMGUI
			debugMaxJumpY = 0;
#endif

			bool didJumpFromWall = false;
			if (jumpFromWallTimer > 0.f) {
				didJumpFromWall = true;
				jumpFromWallTimer = 0.f;
				if (onWall) {
					onWall = false;
					lookingLeft = !lookingLeft;
				}
				vel.x = lookingLeft ? -kVelWalljump : kVelWalljump;
				Particles::DoDustWallJump(pos, vel.x < 0);
			} else if (groundTile.isSlope()) {
				// Jump a bit sideways when on slope
				if (groundTile.isRightSlope()) {
					if (vel.x >= -kVelSlopejump) {
						vel.x = -kVelSlopejump;
					}
				}
				else {
					if (vel.x <= kVelSlopejump) {
						vel.x = kVelSlopejump;
					}
				}
			}

			if (Input::IsPressed(0, GameKeys::CROUCH)) {
				// Allow jumping down one-way tiles faster if jump & crouch are pressed
				crouchedTime = kTimeCrouchedToJumpDownOneWayTile;
			}
			else {
				Sound::Stop(voiceSoundChannel);
				voiceSoundChannel = Assets::soundVoiceJump.Play();
				// Start to jump
				initialJumpY = pos.y;
				float halfWidth = kStandingSize.x / 2;
				Tile topLeft = map->GetTile(Tile::ToTiles(pos.x - halfWidth + 1.f, pos.y - size.y - 1.f));
				Tile topRight = map->GetTile(Tile::ToTiles(pos.x + halfWidth - 1.f, pos.y - size.y - 1.f));
				if (!didJumpFromWall) {
					bool hasBlockAbove = topLeft.isSolid() || topRight.isSolid();
					if (!hasBlockAbove) {
						Particles::DoDustJump(pos);
						groundTile = Tile::NONE;
					}
				}
				crouched = false;
			}
		}

		UpdateMoving(dt);
	}
	else if (diving && groundTile == Tile::NONE) {
		if (Input::IsPressed(0, GameKeys::LEFT)) {
			vel.x -= kDiveHorizontalAcc * dt;
		}
		if (Input::IsPressed(0, GameKeys::RIGHT)) {
			vel.x += kDiveHorizontalAcc * dt;
		}
	}

	// Clamp vel

	float maxVelX = dashing ? kVelDash : kVelMax.x;
	if (vel.x < -maxVelX) vel.x = -maxVelX;
	if (vel.x > maxVelX) vel.x = maxVelX;
	if (vel.y < -kVelMax.y) vel.y = -kVelMax.y;
	float maxVelDown = diving ? kVelDive : kVelMax.y;
	if (vel.y > maxVelDown) vel.y = kVelMax.y;
	if (initialJumpY < Mates::MaxFloat && vel.y < 0) {
		// Do not allow to jump higher than kJumpHeightDiff
		float targetY = initialJumpY - kJumpHeightDiff;
		if (pos.y + vel.y * dt < targetY) {
			vel.y = (targetY - pos.y) / dt;
		};
	}

	// Do move
	MoveResult moved = MoveAgainstTileMap(pos - vec(0, size.y/2), size, vel, dt);
	pos = moved.pos + vec(0, size.y/2);

	bool displayWallSlideAnimEarly = false;

	if (moved.leftWallCollision != Tile::NONE) {
		vel.x = 0;
		dashing = false;
		lookingLeft = true;

		if ((onWall || groundTile == Tile::NONE) && !isHit() && SkillTree::instance()->IsEnabled(Skill::WALLJUMP) && !moved.leftWallCollision.isBreakable(Tile::BreakResistance::SOFT)) {
			if (vel.y > 0) {
				if (!onWall && attacking && anim.current_frame > 0) {
					attacking = false;
				}
				onWall = true;
				initialJumpY = Mates::MaxFloat;
				vel.x = -kFrictAcc_OnAir * 2 * dt; //stay against wall
			}
			else if (vel.y > -80.f) {
				displayWallSlideAnimEarly = true;
			}
		}
	}
	else if (moved.rightWallCollision != Tile::NONE) {
		vel.x = 0;
		dashing = false;
		lookingLeft = false;
		if ((onWall || groundTile == Tile::NONE) && !isHit() && SkillTree::instance()->IsEnabled(Skill::WALLJUMP) && !moved.rightWallCollision.isBreakable(Tile::BreakResistance::SOFT)) {
			if (vel.y > 0) {
				if (!onWall && attacking && anim.current_frame > 0) {
					attacking = false;
				}
				onWall = true;
				initialJumpY = Mates::MaxFloat;
				vel.x = kFrictAcc_OnAir * 2 * dt; //stay against wall
			}
			else if (vel.y > -80.f) {
				displayWallSlideAnimEarly = true;
			}
		}
	}
	else {
		onWall = false;
	}

	if (moved.ceilingCollision != Tile::NONE) {
		vel.y = 0;
		initialJumpY = Mates::MaxFloat;
	}
	if (moved.groundCollision != Tile::NONE) {
		bool destroyingGround = false;
		bool actuallyDiving = diving && divingRestTimer <= 0.f;
		if (actuallyDiving) {
			if (moved.groundCollision.isBreakableGround() || moved.groundCollision.isBreakable(SkillTree::instance()->GetBreakPower())) {
				Fx::Screenshake::StartPreset(Fx::Screenshake::Preset::Stomp);
				bool respawnTiles = !moved.groundCollision.isBreakableGround();
				DestroyedTiles::instance()->Destroy(moved.groundCollisionPos.x, moved.groundCollisionPos.y, respawnTiles);
				pos.y += 1; // move past the floor we just destroyed
				destroyingGround = true;
			}
			else {
				// end dive
				divingRestTimer = kDiveRestTime;
				Fx::Screenshake::StartPreset(Fx::Screenshake::Preset::LittleStomp);
				anim.Ensure(AnimLib::WARRIOR_STANDUP, false);
			}
		}
		if (moved.groundCollision.isOneWay() && crouchedTime >= kTimeCrouchedToJumpDownOneWayTile) {
			Sound::Stop(voiceSoundChannel);
			voiceSoundChannel = Assets::soundVoiceJump.Play();
			pos.y += 3.f;
			vel.y = 0;
			crouched = false;
			groundTile = Tile::NONE;
		}
		else if (!destroyingGround) {
			if (vel.y > 50) {
				Assets::soundStep.Play();
				Particles::DoDustLand(pos);
			}
			vel.y = 0;
			onWall = false;
			groundTile = moved.groundCollision;
			if (moved.groundCollision.isSafeGround()) {
				lastSafeTilePos = moved.groundCollisionPos;
			}
		}
	}
	else {
		if (dashing) {
			// When dashing we aren't pushed down by gravity, so we will not collide with the ground and get to this else even when there is ground
			// Logic to detect if we are on a ground tile is copied from MoveAgainstTileMap()
			veci tilePos = Tile::ToTiles(pos.x, pos.y + 2.f);
			Tile tile = GaemTileMap::instance()->GetTile(tilePos);
			bool oneWayCollision = (tile.isOneWay() && pos.y - 1.f < (tilePos.y * Tile::Size));
			if (tile.isSolid() || oneWayCollision) {
				groundTile = tile;
			}
		}
		else {
			groundTile = Tile::NONE;
		}
	}

	if (diving)
	{
		size = kCrouchedSize;
	}
	else if (dashing || attacking) 
	{
		size = kStandingSize;
		if (dashing && groundTile != Tile::NONE) {
			Particles::DoDustRun(pos, dt, lookingLeft, true);
		}
	} 
	else 
	{
		bool isWalking = false;
		bool isTurning = false;
		if (crouched)
		{
			size = kCrouchedSize;
			anim.Ensure(AnimLib::WARRIOR_CROUCH, false);
		}
		else
		{

			size = kStandingSize;
			if (groundTile != Tile::NONE && vel.y >= 0) // if vel.y < 0 we reached ground from below, we are jumping and not actually grounded
			{
				if (Input::IsPressed(0, GameKeys::LEFT) && !Input::IsPressed(0, GameKeys::RIGHT))
				{
					isWalking = true;
					if (vel.x > 0.f) {
						anim.Ensure(AnimLib::WARRIOR_TURN);
						isTurning = true;
					}
					else {
						anim.Ensure(AnimLib::WARRIOR_RUN);
						if (animationNewFrame && (anim.current_frame == 6 || anim.current_frame == 2)) {
							//Assets::soundLeaves.Play();
						}
					}
				}
				else if (Input::IsPressed(0, GameKeys::RIGHT) && !Input::IsPressed(0, GameKeys::LEFT))
				{
					isWalking = true;
					if (vel.x < 0.f) {
						anim.Ensure(AnimLib::WARRIOR_TURN);
						isTurning = true;
					}
					else {
						anim.Ensure(AnimLib::WARRIOR_RUN);
						if (animationNewFrame && (anim.current_frame == 6 || anim.current_frame == 2)) {
							//Assets::soundLeaves.Play();
						}
					}
				}
				else
				{
					anim.Ensure(AnimLib::WARRIOR_IDLE);
				}
			}
			else
			{
				if (onWall || displayWallSlideAnimEarly) {
					anim.Ensure(AnimLib::WARRIOR_WALL_SLIDE);
					if (displayWallSlideAnimEarly) {
						anim.current_frame = 2;
						anim.timer = 0.f;
					}
				}
				else if (!anim.IsSet(AnimLib::WARRIOR_HURT) || invencibleTimer <= 0.f) {
					if (vel.y <= kVelJump) {
						anim.Ensure(AnimLib::WARRIOR_JUMP, false);
					}
					else if (anim.IsSet(AnimLib::WARRIOR_FALL)) {
						anim.Ensure(AnimLib::WARRIOR_FALL);
					}
					else
					{
						anim.Ensure(AnimLib::WARRIOR_JUMP_TO_FALL, false);
						if (anim.IsComplete()) {
							anim.Ensure(AnimLib::WARRIOR_FALL);
						}
					}
				}
			}
		}
		if (isWalking) {
			Particles::DoDustRun(pos, dt, Input::IsPressed(0, GameKeys::LEFT), isTurning);
		}
	}

	if (SkillTree::instance()->IsEnabled(Skill::GUN)) {
		bfgPos = pos + bfgOffset();
		bfgAngle = bfgPos.AngleDegs(Input::GetAnalog(0, AnalogInput::AIM));
		// Discretize to 8 directions
		//bfgAngle = (int(bfgAngle + (45.f / 2)) / 45) * 45.f;
		if (bfgCooldownTimer > 0.f) {
			bfgCooldownTimer -= dt;
			if (bfgCooldownTimer < 0.f) {
				bfgCooldownTimer = 0.f;
			}
		}
		else if (Input::IsPressed(0, GameKeys::SHOOT)) {
			bfgCooldownTimer = kBfgCooldown;
			vec gunDirection = vec::FromAngleDegs(bfgAngle);
			vec tipOfTheGun = bfgPos + gunDirection*16.f;
			new Bullet(tipOfTheGun, gunDirection*kBulletVel);
			float oldVelY = vel.y;
			vel -= gunDirection*kBfgPushBack;
			initialJumpY = Mates::MaxFloat; // Overrides jump impulse
			if (onWall) {
				vel.x = 0; // Will let wall go if we shoot and we aren't explicitly moving towards the wall
			}
			if (groundTile != Tile::NONE) {
				vel.y = oldVelY; // Do not push towards the ground to prevent playing the "land" sound & particles
				if (abs(vel.x) < 0.1f) {
					Particles::DoDustLand(pos);
				}
				else {
					Particles::DoDustRun(pos, dt, vel.x < 0, true);
					Particles::DoDustRun(pos, dt, vel.x < 0, true);
				}
			}
		}
	}

	if (invencibleTimer > 0.f) {
		invencibleTimer -= dt;
	}

	// We use mainClock so it also counts while the screen is frozen, when you are hit. Otherwise, when hit the animation would last longer.
	if (healthAnimationTimer < mainClock) {
		healthAnimationOldHealth = health;
	}
}

static float toSafeGroundLambdaTimer;
void Player::ToSafeGround() {
	invencibleTimer = kInvencibleTimeAfterHit;
	initialJumpY = Mates::MaxFloat;
	onWall = false;
	crouched = false;
	if (health > 0) {
		invencibleTimer = 0.f;
		anim.Ensure(AnimLib::WARRIOR_FALL, false);
		vec newPos = SafeGroundPos();
		toSafeGroundLambdaTimer = 0.f;
		auto previousUpdateWhileFrozen = Fx::FreezeImage::GetAlternativeUpdateFnWhileFrozen();
		Fx::FreezeImage::Freeze(0.5f);
		Fx::FreezeImage::SetAlternativeUpdateFnWhileFrozen([this, newPos, previousUpdateWhileFrozen](float dt) {
			toSafeGroundLambdaTimer += dt;
			pos = Mates::Lerp(pos, newPos, toSafeGroundLambdaTimer);
			if (previousUpdateWhileFrozen) {
				previousUpdateWhileFrozen(dt);
			}
		});
		Fx::FreezeImage::SetUnfreezeCallback([previousUpdateWhileFrozen]() {
			Fx::FreezeImage::SetAlternativeUpdateFnWhileFrozen(previousUpdateWhileFrozen);
		});
		vel = vec::Zero;
		frozen = false;
	}
}

void Player::TakeDamageFromLava() {
	anim.Ensure(AnimLib::WARRIOR_HURT, false);
	frozen = true; // disable movement
	invencibleTimer = 1;
	bfgPos.y = -1000;
	onWall = false;
	crouched = false;
	dashing = false;
	diving = false;
	attacking = false;
	health--;
	healthAnimationTimer = mainClock + kHealthAnimationTime;

}

void Player::TakeDamage(vec src) {
	invencibleTimer = kInvencibleTimeAfterHit;
	if (pos.x > src.x) {
		vel.x = kTakeDamageKnockbackVel.x;
	}
	else {
		vel.x = -kTakeDamageKnockbackVel.x;
	}
	if (groundTile != Tile::NONE) {
		vel.y = kTakeDamageKnockbackVel.y;
	}
	initialJumpY = Mates::MaxFloat;
	onWall = false;
	crouched = false;
	dashing = false;
	diving = false;
	divingRestTimer = 0.f;
	attacking = false;
	Fx::FreezeImage::Freeze(0.25f);
	justHit = true;
	anim.Ensure(AnimLib::WARRIOR_HURT, false);
	Sound::Stop(voiceSoundChannel);
	voiceSoundChannel = Assets::soundVoiceDamage.Play();
	vec playerCenter = CenterPos();
	float direction = (playerCenter-src).AngleDegs();
	new BackgroundOneShotAnim(Assets::spritesheetTexture, playerCenter, AnimLib::HIT_SPLASH, 2, direction);
	health--;
	healthAnimationTimer = mainClock + kHealthAnimationTime;
}

BoxBounds Player::MaxBounds() const
{
	return BoxBounds(pos, kStandingSize, vec(kStandingSize.x / 2, kStandingSize.y));
}

void Player::Draw() const {

	if (isHit()) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
	}

	GPU_Rect rect = anim.CurrentFrameRect();
	if (!SkillTree::instance()->IsEnabled(Skill::ATTACK) && !SkillTree::instance()->IsEnabled(Skill::WALLJUMP)) { // Walljump doesn't have animations without sword
		rect.x += kOffsetSwordlessSpritesheet;
	}

	Window::Draw(Assets::warriorTexture, pos)
		.withOrigin(AnimLib::warriorSheet.sprite_w/2, AnimLib::warriorSheet.sprite_h-4)
		.withRect(rect)
		.withScale(lookingLeft? -1.f : 1.f, 1.f);

	//BFG
	if (SkillTree::instance()->IsEnabled(Skill::GUN)) {
		GPU_Rect rect;
		if (bfgCooldownTimer > (kBfgCooldown - kBfgCooldown / 4.f)) {
			bool blink = (int(mainClock * 100) % 2);
			rect = { blink ? 32 * 3.f : 32 * 2.f, 3 * 16.f, 2 * 16.f, 16.f };
		}
		else {
			bool shake = (int(mainClock * 6) % 10) > 8;
			rect = { shake ? 32 : 0.f, 3 * 16.f, 2 * 16.f, 16.f };
		}
		float scale = (0.333f + (std::max(kBfgCooldown / 1.5f, bfgCooldownTimer) / kBfgCooldown));
		vec vscale = vec(scale, scale);
		if (bfgAngle < -90 || bfgAngle > 90) {
			vscale.y = -vscale.y;
		}
		Window::Draw(Assets::spritesheetTexture, bfgPos)
			.withRect(rect)
			.withOrigin(10, 8)
			.withRotationDegs(bfgAngle)
			.withScale(vscale);
	}

	Shader::Deactivate();

	// Debug-only
	HitBoxBounds().DebugDraw();
	//Input::GetAnalog(0, AnalogInput::AIM).DebugDraw();
	pos.DebugDraw();
	//CenterPos().DebugDraw();
	if (playerAttack.alive) {
		playerAttack.Bounds().DebugDraw(0,255,0);
	}
}

void Player::Heal(int n) {
	health += n;
	healthAnimationTimer = mainClock + kHealthAnimationTime;
	Mates::ClampMax(health, maxHealth);
}

void Player::HealthUp(int n) {
	health += n;
	healthAnimationTimer = mainClock + kHealthAnimationTime;
	maxHealth += n;
}

namespace AnimLib {
	constexpr const GPU_Rect HEALTH_FOREGROUND = { 495, 260, 1, 9 };
	constexpr const GPU_Rect HEALTH_BACKGROUND = { 497, 260, 1, 9 };
	constexpr const GPU_Rect HEALTH_HIGHLIGHTED = { 499, 260, 1, 9 };
	constexpr const GPU_Rect HEALTH_BEGIN = { 463 + 18, 260, 4, 9 };
	constexpr const GPU_Rect HEALTH_MIDDLE = { 466 + 18, 260, 1, 9 };
	constexpr const GPU_Rect HEALTH_END = { 473 + 18, 260, 4, 9 };
}

// Duplicated from DrawBossHealth
void Player::DrawGUI(bool discreteHealth)
{
#ifdef _IMGUI
	{
		ImGui::Begin("player");
		static bool invincible = false;
		ImGui::Checkbox("invincible", &invincible);
		if (invincible) {
			*(const_cast<int*>(&health)) = std::max(health, maxHealth);
		}
		int prevHealth = health;
		if (ImGui::SliderInt("health", const_cast<int*>(&health), 0, 10)) {
			healthAnimationTimer = mainClock + kHealthAnimationTime;
		};
		vec mapSize = GaemTileMap::instance()->BoundsInWorld().BottomRight();
		ImGui::SliderFloat2("pos", (float*)&pos, 0, std::max(mapSize.x, mapSize.y));
		if (initialJumpY < Mates::MaxFloat) {
			debugMaxJumpY = std::max(debugMaxJumpY, -(pos.y - initialJumpY));
		}
		ImGui::Text("jump height %f", debugMaxJumpY);
		ImGui::Text("vel %f,%f", vel.x, vel.y);
		ImGui::Text("divingRestTimer: %f", divingRestTimer);
		ImGui::Text("ground: %d wall: %d attacking: %d diving: %d dashing: %d slope: %d", groundTile != Tile::NONE, onWall, attacking, diving, dashing, groundTile.isSlope());
		ImGui::End();
	}
#endif

	if (discreteHealth) {
		for (int i = 0; i < maxHealth; i++) {
			float scale = 1.5f;
			if (health == 1) {
				scale += std::abs(Mates::Clamped(sin(mainClock * 6), -0.6f, 9.f)) / 3;
			}
			Window::Draw(Assets::spritesheetTexture, 18 + 24 * i, 18)
				.withRectWithOriginCentered(i < health ? AnimLib::HEALTH_FULL : AnimLib::HEALTH_EMPTY)
				.withScale(scale);
		}
	}
	else {
		float scale = 1.5f;
		const vec pos(6, 28 - 19);
		const int size = 8 * maxHealth;

		// Background
		Window::Draw(Assets::spritesheetTexture, pos + vec(2 * scale, 0))
			.withScale(size * scale, scale)
			.withRect(AnimLib::HEALTH_BACKGROUND);

		// Health
		int healthScale = (float(healthAnimationOldHealth) / maxHealth) * size * scale;
		Window::Draw(Assets::spritesheetTexture, pos + vec(2 * scale, 0))
			.withScale(healthScale, scale)
			.withRect(AnimLib::HEALTH_FOREGROUND);

		// Health variation
		if (healthAnimationOldHealth != health) {
			Window::Draw(Assets::spritesheetTexture, pos + vec(2 * scale + healthScale, 0))
				.withScale((float(health - healthAnimationOldHealth) / maxHealth) * size * scale, scale)
				.withRect(AnimLib::HEALTH_HIGHLIGHTED);
		}

		// Bar begin
		Window::Draw(Assets::spritesheetTexture, pos + vec())
			.withScale(scale)
			.withRect(AnimLib::HEALTH_BEGIN);

		// Bar middle
		Window::Draw(Assets::spritesheetTexture, pos + vec(2 * scale, 0))
			.withScale(size * scale, scale)
			.withRect(AnimLib::HEALTH_MIDDLE);

		// Bar end
		Window::Draw(Assets::spritesheetTexture, pos + vec(size * scale, 0))
			.withScale(scale)
			.withRect(AnimLib::HEALTH_END);
	}
}
