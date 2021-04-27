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

// dash
const float kVelDash = 350;
const float kDashCooldown = 0.6f;
const float kDashDuration = 0.25f;

// dive
const float kVelDive = 450;
const float kDiveRestTime = 0.4f; // time you can't move after touching ground

// limits
const vec kVelMax(220, 350);

// bfg
const float kBulletVel = 400.f;
const float kBfgCooldown = 0.6f;
const float kBfgPushBack = 150.f;

// damage
const vec kKnockbackVel(180.f, -150.f);
const float kInvencibleTimeAfterHit = 0.5f;

// sword attack
const float kSwordAttackRadius = 22.5f;
const vec kSwordAttackOffset = vec(15.5f,-17.f);

const float kSwordAttackDownRadius = 10.f;
const vec kSwordAttackDownOffset = vec(3.f,2.f);

// Sprite
const vec kStandingSize = vec(14, 32);
const vec kCrouchedSize = vec(16, 22);

JumpMan::JumpMan()
	: polvito(Assets::spritesheetTexture)
	, playerAttack(vec::Zero, kSwordAttackRadius)
	, animation(AnimLib::WARRIOR_IDLE)
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

void JumpMan::UpdateMoving(float dt) 
{
	GaemTileMap* map = GaemTileMap::instance();

	// JUMPERINO
	if (Input::IsJustPressed(0,GameKeys::JUMP, 0.15f) && (grounded || onWall)) {
		Input::ConsumeJustPressed(0, GameKeys::JUMP);
		
		jumpTimeLeft = kJumpTime; // the jump upwards velocity can last up to this duration
		if (onWall) {
			onWall = false;
			lookingLeft = !lookingLeft;
			vel.x = lookingLeft? -kVelWalljump : kVelWalljump;
			DoPolvitoWallJump();
		}
		float halfWidth = kStandingSize.x / 2;
		Tile topLeft = map->GetTile(Tile::ToTiles(pos.x - halfWidth + 1.f, pos.y - size.y - 1.f));
		Tile topRight = map->GetTile(Tile::ToTiles(pos.x + halfWidth - 1.f, pos.y - size.y - 1.f));
		bool hasBlockAbove = topLeft.isSolid() || topRight.isSolid();
		if (!hasBlockAbove) {
			DoPolvitoJump();
			grounded = false;
		}
		crouched = false;
	}

	crouched = (grounded && Input::IsPressed(0, GameKeys::CROUCH));
	if (crouched) {
		crouchedTime += dt;
	}
	else {
		crouchedTime = 0.f;
	}

	vec acc = vec(0, 0);
	if (Input::IsPressed(0,GameKeys::LEFT)) {
		if (!attacking) lookingLeft = true;
		if (grounded) {
			if (!crouched) acc.x -= kRunAcc;
		}
		else {
			acc.x -= kRunAcc_OnAir;
		}
	}
	if (Input::IsPressed(0,GameKeys::RIGHT)) {
		if (!attacking) lookingLeft = false;
		if (grounded) {
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
	if (grounded)
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

	// Clamp vel
	if (vel.x > kVelMax.x) vel.x = kVelMax.x;
	if (vel.x < -kVelMax.x) vel.x = -kVelMax.x;
	if (vel.y > kVelMax.y) vel.y = kVelMax.y;
	if (vel.y < -kVelMax.y) vel.y = -kVelMax.y;
}

void JumpMan::Update(float dt)
{
	if (frozen || !alive) return;

	animation.Update(dt);

	veci groundTilePos(-1, -1);
	grounded = IsGrounded(pos - vec(0, size.y / 2), size, &groundTilePos);
	if (grounded) {
		Tile groundTile = GaemTileMap::instance()->GetTileUnsafe(groundTilePos);
		if (groundTile.isSafeGround()) {
			lastSafeTilePos = groundTilePos;
		}
	}

	if (grounded || onWall) {
		canDash = true;
	}

	if (grounded) {
		onWall = false;
	}

	dashCooldown -= dt;
	if (!diving && SkillTree::instance()->IsEnabled(Skill::DASH) && canDash && dashCooldown <= 0.f) {
		if (Input::IsJustPressed(0, GameKeys::DASH, 0.15f)) {
			Input::ConsumeJustPressed(0, GameKeys::DASH);
			dashCooldown = kDashCooldown;
			dashTimer = 0.f;
			dashing = true;
			if (onWall) {
				lookingLeft = !lookingLeft;
			}
			jumpTimeLeft = 0;
			vel = vec(lookingLeft? -kVelDash : kVelDash, 0);
			attacking = false;
			animation.Ensure(AnimLib::WARRIOR_DASH);
		}
	}

	if (!dashing && SkillTree::instance()->IsEnabled(Skill::DIVE)) {
		if (!grounded && Input::IsPressed(0, GameKeys::CROUCH) && Input::IsJustPressed(0, GameKeys::ATTACK, 0.15f)) {
			Input::ConsumeJustPressed(0, GameKeys::ATTACK);
			diving = true;
			attacking = false;
			animation.Ensure(AnimLib::WARRIOR_ATTACK_DOWN_TRANSITION, false);
		}
	}

	if (!dashing && !diving && SkillTree::instance()->IsEnabled(Skill::ATTACK)) {
		if (Input::IsJustPressed(0, GameKeys::ATTACK, 0.15f)) {
			Input::ConsumeJustPressed(0, GameKeys::ATTACK);
			attacking = true;
			animation.Ensure(AnimLib::WARRIOR_MOVING_ATTACK, false);
		}
	}

	if (dashing) {
		dashTimer += dt;
		if (dashTimer >= kDashDuration) {
			dashing = false;
		}
	}

	if (attacking) {
		// TODO: When on wall, attack oposite side
		playerAttack.alive = (animation.CurrentFrameNumber() == 1);
		playerAttack.radius = kSwordAttackRadius;
		playerAttack.pos = pos + kSwordAttackOffset.Mirrored(lookingLeft, false);
		if (animation.IsComplete()) {
			/*if (Input::IsJustPressed(0, GameKeys::ATTACK, 0.15f)) {
				Input::ConsumeJustPressed(0, GameKeys::ATTACK);
				animation.Ensure(AnimLib::WARRIOR_COMBO, false);
			}
			else */
			{
				attacking = false;
			}
		}
	} else if (diving) {
		if (divingRestTimer > 0.f) {
			divingRestTimer -= dt;
			playerAttack.alive = false;
			if (divingRestTimer <= 0.f) {
				diving = false;
			}
			if (animation.complete) { //standup anim done
				animation.Ensure(AnimLib::WARRIOR_IDLE);
			}
		} else if (animation.complete) {
			vel = vec(0, kVelDive);
			animation.Ensure(AnimLib::WARRIOR_ATTACK_DOWN);
			playerAttack.radius = kSwordAttackDownRadius;
			playerAttack.alive = true;
			//Debug::FrameByFrame = true;
		}
		playerAttack.pos = pos + kSwordAttackDownOffset.Mirrored(lookingLeft, false);
	} else {
		playerAttack.alive = false;
	}

	if (!dashing && !diving) {
		UpdateMoving(dt);
	}
	
	// Do move
	MoveResult moved = MoveAgainstTileMap(pos - vec(0, size.y/2), size, vel, dt);
	pos = moved.pos + vec(0, size.y/2);

	if (moved.leftWallCollision != Tile::NONE) {
		if (!attacking && !isHit() && SkillTree::instance()->IsEnabled(Skill::WALLJUMP)) {
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
		if (!attacking && !isHit() && SkillTree::instance()->IsEnabled(Skill::WALLJUMP)) {
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
			if (moved.groundCollision.isBreakableGround()) {
				Fx::Screenshake::StartPreset(Fx::Screenshake::Preset::Stomp);
				DestroyedTiles::instance()->Destroy(moved.groundCollisionPos.x, moved.groundCollisionPos.y, true, false);
				pos.y += 1; // move past the floor we just destroyed
				destroyingGround = true;
			}
			else {
				// end dive
				divingRestTimer = kDiveRestTime;
				Fx::Screenshake::StartPreset(Fx::Screenshake::Preset::LittleStomp);
				animation.Ensure(AnimLib::WARRIOR_STANDUP, false);
			}
		}
		if (moved.groundCollision.isOneWay() && crouchedTime > kTimeCrouchedToJumpDownOneWayTile) {
			pos.y += 3.f;
			vel.y = 0;
			crouched = false;
			grounded = false;
		}
		else if (!destroyingGround) {
			if (vel.y > 50) DoPolvitoLand();
			if (moved.groundCollision.isSlope()) {
				vel.y = 30.f; //this helps you get grounded as soon as the slope ends
			}
			else {
				vel.y = 0;
			}
			onWall = false;
			grounded = true;
		}
	}

	if (diving)
	{
		size = kCrouchedSize;
	}
	else if (dashing || attacking) 
	{
		size = kStandingSize;
		if (dashing && grounded) {
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
			animation.Ensure(AnimLib::WARRIOR_CROUCH, false);
		}
		else
		{

			size = kStandingSize;
			if (grounded)
			{
				if (Input::IsPressed(0, GameKeys::LEFT) && !Input::IsPressed(0, GameKeys::RIGHT))
				{
					isWalking = true;
					if (vel.x > 0.f) {
						animation.Ensure(AnimLib::WARRIOR_TURN);
						isTurning = true;
					}
					else {
						animation.Ensure(AnimLib::WARRIOR_RUN);
					}
				}
				else if (Input::IsPressed(0, GameKeys::RIGHT) && !Input::IsPressed(0, GameKeys::LEFT))
				{
					isWalking = true;
					if (vel.x < 0.f) {
						animation.Ensure(AnimLib::WARRIOR_TURN);
						isTurning = true;
					}
					else animation.Ensure(AnimLib::WARRIOR_RUN);
				}
				else
				{
					animation.Ensure(AnimLib::WARRIOR_IDLE);
				}
			}
			else
			{
				if (onWall) {
					animation.Ensure(AnimLib::WARRIOR_WALL_SLIDE);
				}
				else if (invencibleTimer > 0.f) {
					animation.Ensure(AnimLib::WARRIOR_HURT, false);
				}
				else if (vel.y <= kVelJump) {
					animation.Ensure(AnimLib::WARRIOR_JUMP, false);
				}
				else if (animation.IsSet(AnimLib::WARRIOR_FALL)) {
					animation.Ensure(AnimLib::WARRIOR_FALL);
				}
				else {
					animation.Ensure(AnimLib::WARRIOR_JUMP_TO_FALL, false);
					if (animation.IsComplete()) {
						animation.Ensure(AnimLib::WARRIOR_FALL);
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
		vel.x = kKnockbackVel.x;
	}
	else {
		vel.x = -kKnockbackVel.x;
	}
	if (grounded) {
		vel.y = kKnockbackVel.y;
	}
	jumpTimeLeft = 0;
	onWall = false;
	crouched = false;
	dashing = false;
	diving = false;
	divingRestTimer = 0.f;
	attacking = false;
	Fx::FreezeImage::Freeze(0.25f);
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
		ImGui::Begin("jumpman scene");
		static bool invincible = false;
		ImGui::Checkbox("invincible", &invincible);
		if (invincible) {
			*(const_cast<int*>(&health)) = std::max(health, maxHealth);
		}
		ImGui::SliderInt("health", const_cast<int*>(&health), 0, 10);
		ImGui::SliderFloat2("pos", (float*)&pos, 16.f, 4500.f);
		ImGui::Text("vel %f,%f", vel.x, vel.y);
		ImGui::Text("jumpTimeLeft: %f attacking: %d diving: %d dashing: %d", jumpTimeLeft, attacking, diving, dashing);
		ImGui::End();
	}
#endif
	polvito.Draw();

	if (isHit()) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
	}

	Window::Draw(Assets::warriorTexture, pos)
		.withOrigin(AnimLib::warriorSheet.sprite_w/2, AnimLib::warriorSheet.sprite_h)
		.withRect(animation.CurrentFrameRect())
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
