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
	, anim(AnimLib::FIRESLIME_WALK, false)
{
	direction = Rand::OnceEvery(2) ? 1 : -1;
	screen = ScreenManager::instance()->FindScreenContaining(pos);
}

BoxBounds FireSlime::AttackBounds() const
{
	BoxBounds bounds = BoxBounds::FromCenter(pos, vec(Tile::size * 9, Tile::size * 5));
	bounds.left += direction * bounds.width / 2;
	bounds.top += bounds.height / 2 - 25;
	return bounds;
}

bool FireSlime::CanMoveForward() const
{
	constexpr int framesOnAir = kFirstFrameOnGround - kFirstFrameOnAir;
	constexpr const float bigDt = Animation::TotalDurationForFrames(AnimLib::FIRESLIME_WALK, kFirstFrameOnAir, framesOnAir);
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
		if (anim.CurrentFrameNumber() == kFrameShooting) {
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
		willAttack = willAttack || (!didJustAttack && Collide(AttackBounds(), JumpMan::instance()->Bounds()));
		if (!anim.IsComplete() && anim.CurrentFrameNumber() >= kFirstFrameOnAir && anim.CurrentFrameNumber() < kFirstFrameOnGround) {
			// in this part of the animation we are in the air, here we move and we never start an attack
			pos.x += kSpeed * direction * dt;
			didJustAttack = false;
		} else if (willAttack) {
			state = State::ATTACKING;
			timer = 0.0f;
			anim.Set(AnimLib::FIRESLIME_ATTACK, false);
			willAttack = false;
		}
		break;
	}

	if (anim.IsComplete()) {
		if (!CanMoveForward()) {
			direction = -direction;
			willAttack = false;
		}
		didJustAttack = (state == State::ATTACKING);
		state = State::WALKING;
		anim.Set(AnimLib::FIRESLIME_WALK, false);
	}

	if (ReceiveDamageFromBullets(Bounds())) {
		DieWithSmallExplosion(this); //single hit
		return;
	}

	DamagePlayerOnCollision(Bounds());

}


void FireSlime::Draw() const
{
	GPU_Rect rect = anim.CurrentFrameRect();

	vec drawPos = pos - vec(0, kSpriteOffsetY/2.f);

	Window::Draw(Assets::marioTexture, drawPos)
		.withRect(rect)
		.withScale(-direction * kSpriteScale, kSpriteScale)
		.withOrigin(rect.w / 2, rect.h / 2);

	// Debug-only
	pos.DebugDraw();
	Bounds().DebugDraw();
	AttackBounds().DebugDraw(255, 0, 255);
}
