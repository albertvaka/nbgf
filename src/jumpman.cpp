#include "jumpman.h"

#include "input.h"
#include "bullet.h"
#include "mates.h"
#include "destroyedtiles.h"
#include "assets.h"
#include "anim_lib.h"
#include "fx.h"
#include "oneshotanim.h"
#include "debug.h"
#include "savestate.h"
#include "common_tilemapcharacter.h"
#include "common_bullet.h"
#include "skilltree.h"
#ifdef _IMGUI
#include "imgui.h"
#endif

extern float mainClock;

// accel
const float kRunAcc = 1400;
const float kRunAcc_OnAir = 400;
const float kGravityAcc = 660;

// friction X
const float kFrictAccFloor = 1000;
const float kFrictAccFloor_Crouched = 450;
const float kFrictAcc_OnAir = 145;

// friction Y
const float kFrictAccVert_WallUp = 1200;
const float kFrictAccVert_WallDown = 450;

// jump
const float kVelJump = -150;
const float kVelWalljump = 90;
const float kJumpTime = 0.35f;
const float kTimeCrouchedToJumpDownOneWayTile = 0.2f;
const float kTimeToJumpFromWallAfterLettingGo = 0.15f;

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
const float kBfgCooldown = 0.6f;
const float kBfgPushBack = 150.f;

// knockback
const float kDoDamageKnockbackVel = 120.f;
const float kDoDamageKnockbackVelGrounded = 180.f;
const float kDoDamageUpKnockbackVel = 200.f;
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
const vec kStandingSize = vec(14, 32);
const vec kCrouchedSize = vec(16, 22);

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

JumpMan::JumpMan()
	: polvito(Assets::spritesheetTexture)
	, playerAttack(vec::Zero, kSwordAttackRadius)
	, anim(AnimLib::WARRIOR_IDLE)
	, size(kStandingSize)
	, lastSafeTilePos(-1,-1)
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

void JumpMan::DealDamage(vec target) {
	if (diving) {
		vel.y = kDoDamageDownKnockbackVel;
		divingRestTimer = kDiveRestTimeAfterHitting;
	}
	else if (attackingUp) {
		vel.y += kDoDamageUpKnockbackVel;
	} else {
		float knockback = groundTile != Tile::NONE ? kDoDamageKnockbackVelGrounded : kDoDamageKnockbackVel;
		if (pos.x > target.x) {
			vel.x /= 2;
			vel.x += knockback;
		}
		else {
			vel.x /= 2;
			vel.x += -knockback;
		}
	}
	jumpTimeLeft = 0;
	onWall = false;
	crouched = false;
	dashing = false;
	diving = false;
}

void JumpMan::UpdateMoving(float dt) 
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

	if (Input::IsPressed(0,GameKeys::JUMP) && jumpTimeLeft > 0)
	{
		vel.y = kVelJump;
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

void JumpMan::Update(float dt)
{
	if (frozen || !alive) return;

	justHit = false;

	anim.Update(dt);


	if (groundTile != Tile::NONE || onWall) {
		canDash = true;
	}

	if (groundTile != Tile::NONE) {
		onWall = false;
	}

	dashCooldown -= dt;
	if (!diving && SkillTree::instance()->IsEnabled(Skill::DASH) && canDash && dashCooldown <= 0.f) {
		if (Input::IsJustPressed(0, GameKeys::DASH, 0.15f)) {
			Input::ConsumeJustPressed(0, GameKeys::DASH);
			dashCooldown = kDashCooldown;
			dashTimer = 0.f;
			dashing = true;
			canDash = false;
			if (onWall) {
				lookingLeft = !lookingLeft;
			}
			jumpTimeLeft = 0;
			vel = vec(lookingLeft? -kVelDash : kVelDash, 0);
			attacking = false;
			anim.Ensure(AnimLib::WARRIOR_DASH);
		}
	}

	if (!dashing && SkillTree::instance()->IsEnabled(Skill::DIVE) && divingRestTimer <= 0.f) {
		if (groundTile == Tile::NONE && Input::IsPressed(0, GameKeys::CROUCH) && Input::IsJustPressed(0, GameKeys::ATTACK, 0.15f)) {
			Input::ConsumeJustPressed(0, GameKeys::ATTACK);
			diving = true;
			attacking = false;
			anim.Ensure(AnimLib::WARRIOR_ATTACK_DOWN_TRANSITION, false);
		}
	}

	if (!dashing && !diving && SkillTree::instance()->IsEnabled(Skill::ATTACK) && (!Input::IsPressed(0, GameKeys::CROUCH) || !SkillTree::instance()->IsEnabled(Skill::DIVE))) {
		if (Input::IsJustPressed(0, GameKeys::ATTACK, 0.15f)) {
			Input::ConsumeJustPressed(0, GameKeys::ATTACK);
			attacking = true;
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

	if (!dashing && !diving) {

		GaemTileMap* map = GaemTileMap::instance();

		// JUMPERINO
		if (Input::IsJustPressed(0, GameKeys::JUMP, 0.15f) && (groundTile != Tile::NONE || jumpFromWallTimer > 0.f)) {
			Input::ConsumeJustPressed(0, GameKeys::JUMP);

			bool didJumpFromWall = false;
			if (jumpFromWallTimer > 0.f) {
				didJumpFromWall = true;
				jumpFromWallTimer = 0.f;
				if (onWall) {
					onWall = false;
					lookingLeft = !lookingLeft;
				}
				vel.x = lookingLeft ? -kVelWalljump : kVelWalljump;
				DoPolvitoWallJump();
			} else if (groundTile.isSlope()) {
				// Jump a bit sideways when on slope
				if (groundTile.isRightSlope()) {
					if (vel.x >= -kVelWalljump) {
						vel.x = -kVelWalljump;
					}
				}
				else {
					if (vel.x <= kVelWalljump) {
						vel.x = kVelWalljump;
					}
				}
			}

			if (Input::IsPressed(0, GameKeys::CROUCH)) {
				// Allow jumping down one-way tiles faster if jump is pressed while crouching
				crouchedTime = kTimeCrouchedToJumpDownOneWayTile;
			}
			else {
				jumpTimeLeft = kJumpTime; // the jump upwards velocity can last up to this duration
				float halfWidth = kStandingSize.x / 2;
				Tile topLeft = map->GetTile(Tile::ToTiles(pos.x - halfWidth + 1.f, pos.y - size.y - 1.f));
				Tile topRight = map->GetTile(Tile::ToTiles(pos.x + halfWidth - 1.f, pos.y - size.y - 1.f));
				if (!didJumpFromWall) {
					bool hasBlockAbove = topLeft.isSolid() || topRight.isSolid();
					if (!hasBlockAbove) {
						DoPolvitoJump();
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
			vel.x -= 10 * dt;
		}
		if (Input::IsPressed(0, GameKeys::RIGHT)) {
			vel.x += 10 * dt;
		}
	}

	// Clamp vel

	float maxVelX = dashing ? kVelDash : kVelMax.x;
	if (vel.x < -maxVelX) vel.x = -maxVelX;
	if (vel.x > maxVelX) vel.x = maxVelX;
	if (vel.y < -kVelMax.y) vel.y = -kVelMax.y;
	float maxVelDown = diving ? kVelDive : kVelMax.y;
	if (vel.y > maxVelDown) vel.y = kVelMax.y;

	// Do move
	MoveResult moved = MoveAgainstTileMap(pos - vec(0, size.y/2), size, vel, dt);
	pos = moved.pos + vec(0, size.y/2);

	if (moved.leftWallCollision != Tile::NONE) {
		if ((onWall || groundTile == Tile::NONE) && !isHit() && SkillTree::instance()->IsEnabled(Skill::WALLJUMP) && !moved.leftWallCollision.isBreakable(Tile::BreakResistance::SOFT)) {
			if (!onWall && attacking && anim.current_frame > 0) {
				attacking = false;
			}
			onWall = true;
			vel.x = -500.f * dt; //stay against wall
		}
		else {
			vel.x = 0;
		}
		dashing = false;
		lookingLeft = true;
	}
	else if (moved.rightWallCollision != Tile::NONE) {
		if ((onWall || groundTile == Tile::NONE) && !isHit() && SkillTree::instance()->IsEnabled(Skill::WALLJUMP) && !moved.rightWallCollision.isBreakable(Tile::BreakResistance::SOFT)) {
			if (!onWall && attacking && anim.current_frame > 0) {
				attacking = false;
			}
			onWall = true;
			vel.x = 500.f * dt; //stay against wall
		}
		else {
			vel.x = 0;
		}
		dashing = false;
		lookingLeft = false;
	}
	else {
		onWall = false;
	}

	if (moved.ceilingCollision != Tile::NONE) {
		vel.y = 0;
		jumpTimeLeft = 0;
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
			pos.y += 3.f;
			vel.y = 0;
			crouched = false;
			groundTile = Tile::NONE;
		}
		else if (!destroyingGround) {
			if (vel.y > 50) DoPolvitoLand();
			vel.y = 0;
			onWall = false;
			groundTile = moved.groundCollision;
			if (moved.groundCollision.isSafeGround()) {
				lastSafeTilePos = moved.groundCollisionPos;
			}
		}
	}
	else {
		groundTile = Tile::NONE;
	}

	if (diving)
	{
		size = kCrouchedSize;
	}
	else if (dashing || attacking) 
	{
		size = kStandingSize;
		if (dashing && groundTile != Tile::NONE) {
			DoPolvitoRun(dt, lookingLeft, true);
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
					}
				}
				else if (Input::IsPressed(0, GameKeys::RIGHT) && !Input::IsPressed(0, GameKeys::LEFT))
				{
					isWalking = true;
					if (vel.x < 0.f) {
						anim.Ensure(AnimLib::WARRIOR_TURN);
						isTurning = true;
					}
					else anim.Ensure(AnimLib::WARRIOR_RUN);
				}
				else
				{
					anim.Ensure(AnimLib::WARRIOR_IDLE);
				}
			}
			else
			{
				if (onWall) {
					anim.Ensure(AnimLib::WARRIOR_WALL_SLIDE);
				}
				else if (invencibleTimer > 0.f) {
					anim.Ensure(AnimLib::WARRIOR_HURT, false);
				}
				else if (vel.y <= kVelJump) {
					anim.Ensure(AnimLib::WARRIOR_JUMP, false);
				}
				else if (anim.IsSet(AnimLib::WARRIOR_FALL)) {
					anim.Ensure(AnimLib::WARRIOR_FALL);
				}
				else {
					anim.Ensure(AnimLib::WARRIOR_JUMP_TO_FALL, false);
					if (anim.IsComplete()) {
						anim.Ensure(AnimLib::WARRIOR_FALL);
					}
				}
			}
		}
		if (isWalking) {
			DoPolvitoRun(dt, Input::IsPressed(0, GameKeys::LEFT), isTurning);
		}
	}

	
	polvito.UpdateParticles(dt);


	if (SkillTree::instance()->IsEnabled(Skill::GUN)) {
		bfgPos = pos + bfgOffset();
		bfgAngle = bfgPos.AngleDegs(Input::GetAnalog(0, AnalogInput::AIM));
		bfgAngle = (int(bfgAngle + 360 + (45.f / 2)) / 45) * 45.f;
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
			vel -= gunDirection*kBfgPushBack;
			jumpTimeLeft = 0; // Overrides jump impulse
			if (onWall) {
				vel.x = 0; // Will let wall go if we shoot and we aren't explicitly moving towards the wall
			}
			if (groundTile != Tile::NONE) {
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

void JumpMan::ToSafeGround() {
	invencibleTimer = kInvencibleTimeAfterHit;
	jumpTimeLeft = 0;
	onWall = false;
	crouched = false;
	if (health > 0) {
		pos = Tile::FromTiles(lastSafeTilePos)+vec(Tile::Size/2,0);
		vel = vec::Zero;
		frozen = false;
	}
}

void JumpMan::TakeDamage(vec src) {
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
	jumpTimeLeft = 0;
	onWall = false;
	crouched = false;
	dashing = false;
	diving = false;
	divingRestTimer = 0.f;
	attacking = false;
	Fx::FreezeImage::Freeze(0.25f);
	justHit = true;

	vec playerCenter = Bounds().Center();
	float direction = (playerCenter-src).AngleDegs();
	new OneShotAnim(Assets::spritesheetTexture, playerCenter, AnimLib::HIT_SPLASH, 2, direction);
	health--;
}

BoxBounds JumpMan::MaxBounds() const
{
	return BoxBounds(pos, kStandingSize, vec(kStandingSize.x / 2, kStandingSize.y));
}

void JumpMan::Draw() const {

#ifdef _IMGUI
	{
		ImGui::Begin("jumpman");
		static bool invincible = false;
		ImGui::Checkbox("invincible", &invincible);
		if (invincible) {
			*(const_cast<int*>(&health)) = std::max(health, maxHealth);
		}
		ImGui::SliderInt("health", const_cast<int*>(&health), 0, 10);
		ImGui::SliderFloat2("pos", (float*)&pos, 16.f, 4500.f);
		ImGui::Text("vel %f,%f", vel.x, vel.y);
		ImGui::Text("jumpTimeLeft: %f divingRestTimer: %f", jumpTimeLeft, divingRestTimer);
		ImGui::Text("ground: %d wall: %d attacking: %d diving: %d dashing: %d slope: %d", groundTile != Tile::NONE, onWall, attacking, diving, dashing, groundTile.isSlope());
		ImGui::End();
	}
#endif
	polvito.Draw();

	if (isHit()) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
	}

	Window::Draw(Assets::warriorTexture, pos)
		.withOrigin(AnimLib::warriorSheet.sprite_w/2, AnimLib::warriorSheet.sprite_h-4)
		.withRect(anim.CurrentFrameRect())
		.withScale(lookingLeft ? -1.f : 1.f, 1.f);

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
		if (bfgAngle < 270 || bfgAngle  > 450) {
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
	Bounds().DebugDraw();
	//Input::GetAnalog(0, AnalogInput::AIM).DebugDraw();
	pos.DebugDraw();
	//Bounds().Center().DebugDraw();
	if (playerAttack.alive) {
		playerAttack.Bounds().DebugDraw(0,255,0);
	}
}

void JumpMan::DrawGUI() const {
	for (int i = 0; i < maxHealth; i++) {
		Window::Draw(Assets::spritesheetTexture, 10 + 16 * i, 10)
			.withRect(i < health ? AnimLib::HEALTH_FULL : AnimLib::HEALTH_EMPTY);
	}
}

// BRILLI-BRILLI

void JumpMan::InitPolvito() {
	polvito.AddSprite(AnimLib::POLVITO_PARTICLE);

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
