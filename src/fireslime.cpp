#include "fireslime.h"

#include "jumpman.h"
#include "collide.h"
#include "window.h"
#include "assets.h"
#include "tilemap.h"
#include "rand.h"
#include "collide.h"
#include "fireshot.h"
#include "common_enemy.h"
#include "common_tilemapcharacter.h"

constexpr const float kSpeed = 25;
constexpr const float kShotsPerAttack = 5;

constexpr const vec kShotSpeed(70, -90);
constexpr const vec kShotAccel(0, 150);

constexpr const float kSpriteScale = 1.3f;
constexpr const float kSpriteOffsetY = 7.f;

constexpr const int kFirstFrameOnAir = 2;
constexpr const int kFirstFrameOnGround = 7;
constexpr const int kFrameShooting = 2;

FireSlime::FireSlime(vec pos)
	: CircleEntity(pos - vec(0, kSpriteOffsetY), 5*kSpriteScale)
	, anim(AnimLib::FIRESLIME_WALK)
{
	direction = Rand::OnceEvery(2) ? 1 : -1;
	anim.loopable = false;
	screen = ScreenManager::instance()->FindScreenContaining(pos);
}

Bounds FireSlime::AttackBounds() const
{
	Bounds bounds = Bounds::fromCenter(pos, vec(Tile::size * 9, Tile::size * 5));
	bounds.left += direction * bounds.width / 2;
	bounds.top += bounds.height / 2 - 25;
	return bounds;
}

bool FireSlime::CanMoveForward() const
{
	constexpr int framesOnAir = kFirstFrameOnGround - kFirstFrameOnAir;
	constexpr const float bigDt = Animation2::GetTotalDurationForFrames(AnimLib::FIRESLIME_WALK, kFirstFrameOnAir, framesOnAir);
	constexpr const vec spriteSize = AnimLib::FIRESLIME_WALK[0].GetSize() * kSpriteScale;
	vec vel = vec(kSpeed * direction, 0);

	return !IsGoingToHitAWall(pos, spriteSize, vel, bigDt) && !IsGoingToRunOffPlatform(pos, spriteSize, vel, bigDt);
}

void FireSlime::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		return;
	}

	anim.Update(dt);

	switch (state)
	{
	case State::ATTACKING:
		if (anim.current_frame == kFrameShooting) {
			timer -= dt;
			constexpr const float anim_duration = AnimLib::FIRESLIME_ATTACK[kFrameShooting].duration;
			constexpr const float time_between_shots = anim_duration / (kShotsPerAttack-1);
			if (timer <= 0) {
				timer += time_between_shots;
				new FireShot(pos, vec(direction * kShotSpeed.x, kShotSpeed.y), kShotAccel);
			}
		}
		break;
	case State::WALKING:
		willAttack = willAttack || (!didJustAttack && Collide(AttackBounds(), JumpMan::instance()->bounds()));
		if (!anim.complete && anim.current_frame >= kFirstFrameOnAir && anim.current_frame < kFirstFrameOnGround) {
			// in this part of the animation we are in the air, here we move and we never start an attack
			pos.x += kSpeed * direction * dt;
			didJustAttack = false;
		} else if (willAttack) {
			state = State::ATTACKING;
			timer = 0.0f;
			anim.Set(AnimLib::FIRESLIME_ATTACK);
			anim.loopable = false;
			willAttack = false;
		}
		break;
	}

	if (anim.complete) {
		if (!CanMoveForward()) {
			direction = -direction;
			willAttack = false;
		}
		didJustAttack = (state == State::ATTACKING);
		state = State::WALKING;
		anim.Set(AnimLib::FIRESLIME_WALK);
		anim.loopable = false;
	}

	if (ReceiveDamageFromBullets(bounds())) {
		die(); //single hit
		return;
	}

	DamagePlayerOnCollision(bounds());

}


void FireSlime::Draw() const
{
	pos.Debuggerino();
	GPU_Rect rect = anim.GetCurrentFrameRect();

	vec drawPos = pos - vec(0, kSpriteOffsetY/2.f);

	Window::Draw(Assets::marioTexture, drawPos)
		.withRect(rect)
		.withScale(-direction * kSpriteScale, kSpriteScale)
		.withOrigin(rect.w / 2, rect.h / 2);

	if (Debug::Draw) {
		bounds().Draw();
		AttackBounds().Draw(255, 0, 255);
	}
}
