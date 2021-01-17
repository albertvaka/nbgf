#include "flyingalien.h"

#include "jumpman.h"
#include "collide.h"
#include "window.h"
#include "assets.h"
#include "shader.h"
#include "rand.h"
#include "common_tilemapcharacter.h"
#include "common_enemy.h"

constexpr const float speed = 25;
constexpr const float speedAttack = 100; 

constexpr const float overshotEndCharge = 10;
constexpr const float overshotReturnSpeed = 50;

constexpr const float hitTime = 0.5f;
constexpr const float prepareAttackTime = 0.5f;
constexpr const float exitAttackTime = 0.2f;

constexpr const float spriteScale = .75f;
constexpr const float spriteRadius = 15.f;

// Area that if intersects with the player will trigger an attack
constexpr const vec playerNearbyArea = vec(100, 100);

FlyingAlien::FlyingAlien(vec pos)
	: CircleEntity(pos - vec(0,8), spriteRadius)
	, state(State::FLYING)
	, anim(AnimLib::FLYING_ALIEN)
{
	orig = this->pos;
	vel.x = Rand::OnceEvery(2)? -speed : speed;
	screen = ScreenManager::instance()->FindScreenContaining(pos);
}

BoxBounds FlyingAlien::ChargeBounds() const
{
	BoxBounds chargeBounds = BoxBounds(pos, playerNearbyArea);
	if (vel.x < 0) {
		chargeBounds.left -= chargeBounds.width;
		chargeBounds.left += spriteRadius;
	}
	else {
		chargeBounds.left -= spriteRadius;
	}
	return chargeBounds;
}

void FlyingAlien::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		return;
	}

	hitTimer -= dt;

	anim.Update(dt);

	JumpMan* player = JumpMan::instance();

	float walkDir = vel.x > 0 ? 1 : -1;

	switch (state)
	{
	case State::CHARGING:
	{
		pos.x += vel.x * dt;
		pos.y = orig.y + walkDir*std::sin(((pos.x - beginAttackX) / playerNearbyArea.x) * M_PI) * playerNearbyArea.y;
		if (walkDir*(pos.x - beginAttackX) > playerNearbyArea.x) {
			timer = 0.f;
			state = State::EXIT_CHARGE;
		}
	}
	break;
	case State::EXIT_CHARGE:
	{
		timer += dt;
		pos.x += Mates::Lerp(speedAttack, speed, (timer / exitAttackTime)) * walkDir * dt;
		pos.y = orig.y - std::sin((timer/exitAttackTime) * M_PI/2) * overshotEndCharge;
		if (timer >= exitAttackTime) {
			state = State::FLYING;
			if (pos.x < player->pos.x) {
				vel.x = speed;
			} 
			else {
				vel.x = -speed;
			}
		}
	}
	break;
	case State::ENTER_CHARGE:
	{
		timer += dt;
		if (timer > prepareAttackTime) {
			state = State::CHARGING;
			beginAttackX = pos.x;
			timer = 0.f;
		}
	}
	break;
	case State::FLYING: 
	{

		BoxBounds chargeBounds = ChargeBounds();

		if (IsGoingToHitAWall(chargeBounds.Center(), chargeBounds.Size(), vel, dt)
			|| IsGoingToLeaveTheScreen(chargeBounds.Center(), chargeBounds.Size(), vel, dt, screen)
		) {
			vel.x = -vel.x;
		}
		pos.x += vel.x * dt;

		if (Mates::IsNearlyEqual(pos.y, orig.y, 0.5f)) {
			if (Collide(player->Bounds(), ChargeBounds())) {
				state = State::ENTER_CHARGE;
				timer = 0.f;

				if (pos.x < player->pos.x) {
					vel.x = speedAttack;
				}
				else {
					vel.x = -speedAttack;
				}

				float bigDtAttack = playerNearbyArea.y / speedAttack; // total time that will be spent attacking
				if (IsGoingToHitAWall(chargeBounds.Center(), chargeBounds.Size(), vel, bigDtAttack)
					|| IsGoingToLeaveTheScreen(chargeBounds.Center(), chargeBounds.Size(), vel, bigDtAttack, screen))
				{
					//Debug::out << "Flipping because would hit a wall during the attack";
					vel.x = -vel.x;
				}
			}
		} else if (pos.y > orig.y) {
			pos.y -= overshotReturnSpeed*dt;
		} else if (pos.y < orig.y) {
			pos.y += overshotReturnSpeed*dt;
		}
	}
	break;
	}

	Bullet* b = ReceiveDamageFromBullets(Bounds());
	if (b) {
		TakeDamage(b->pos);
		if (alive == false) return;
	}

	DamagePlayerOnCollision(Bounds());
}

void FlyingAlien::TakeDamage(vec src) {
	hitTimer = hitTime;

	if (state == State::FLYING && !IsMovingTowardsInX(pos, vel, src)) {
		vel.x = -vel.x;
	}

	health--;
	if (health <= 0) {
		DieWithSmallExplosion(this);
	}
}

void FlyingAlien::Draw() const
{
	if (hitTimer > 0.f) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
	}

	vec drawPos = pos + vec(vel.x > 0? 2 : -2,(std::sin((orig.x + pos.x)*0.1f)*4) - 3);
	if (state == State::ENTER_CHARGE) {
		drawPos.y -= sinf((timer / prepareAttackTime) * M_PI) * Tile::size;
	}

	GPU_Rect rect = anim.GetCurrentFrameRect();
	Window::Draw(Assets::scifiTexture, drawPos)
		.withRect(rect)
		.withScale(vel.x> 0? -spriteScale : spriteScale, spriteScale)
		.withOrigin(rect.w / 2, rect.h / 2);

	Shader::Deactivate();

	// Debug-only
	pos.DebugDraw();
	vec(pos.x, orig.y).DebugDraw(0, 0, 255);
	Bounds().DebugDraw();
	ChargeBounds().DebugDraw(255, 0, 255);
}
