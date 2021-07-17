#include "fireslime.h"

#include "jumpman.h"
#include "collide.h"
#include "window.h"
#include "assets.h"
#include "rand.h"
#include "fireshot.h"
#include "common_enemy.h"
#include "common_tilemapcharacter.h"

constexpr const float kSpeed = 25;

constexpr const vec kShotSpeed(70, -90);
constexpr const vec kShotAccel(0, 150);

constexpr const float kSpriteScale = 1.3f;
constexpr const float kSpriteOffsetY = 7.f;

constexpr const int kFirstFrameOnAir = 2;
constexpr const int kFirstFrameOnGround = 7;
constexpr const int kFrameShooting = 2;

constexpr const float kShotsPerAttack = 4;
constexpr const float kAttackFrameDuration = AnimLib::FIRESLIME_ATTACK[kFrameShooting].duration;
constexpr const float kTimeBetweenShots = kAttackFrameDuration / kShotsPerAttack;

constexpr const vec kGroundCollision = vec(10, 10) * kSpriteScale;

FireSlime::FireSlime(vec pos)
	: CircleEntity(pos - vec(0, kSpriteOffsetY), 5*kSpriteScale)
	, anim(AnimLib::FIRESLIME_WALK, false)
{
	direction = Rand::OnceEvery(2) ? 1 : -1;
	screen = ScreenManager::instance()->FindScreenContaining(pos);

	this->pos = AlignWithGround(this->pos, kGroundCollision);
}

BoxBounds FireSlime::AttackBounds() const
{
	BoxBounds bounds = BoxBounds::FromCenter(pos, vec(Tile::Size * 9, Tile::Size * 5));
	bounds.left += direction * bounds.width / 2;
	bounds.top += bounds.height / 2 - 25;
	return bounds;
}

bool FireSlime::CanMoveForward() const
{
	constexpr int framesOnAir = kFirstFrameOnGround - kFirstFrameOnAir;
	constexpr const float bigDt = Animation::TotalDurationForFrames(AnimLib::FIRESLIME_WALK, kFirstFrameOnAir, framesOnAir);
	vec vel = vec(kSpeed * direction, 0);

	return !IsGoingToHitAWall(pos, kGroundCollision, vel, bigDt) && !IsGoingToRunOffPlatform(pos, kGroundCollision, vel, bigDt);
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
			if (timer <= 0.f) {
				if (anim.CurrentFrameNumber() == kFrameShooting) {
					timer += kTimeBetweenShots;
				}
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
			timer = kTimeBetweenShots/2;
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

	if (ReceiveDamageFromPlayer(Bounds(), false)) {
		DieWithSmallExplosion(this); //single hit
		return;
	}

	DamagePlayerOnCollision(Bounds());

}


void FireSlime::Draw() const
{
	GPU_Rect rect = anim.CurrentFrameRect();

	vec drawPos = pos - vec(0, kSpriteOffsetY/2.f);

	Window::Draw(Assets::spritesheetTexture, drawPos)
		.withRect(rect)
		.withScale(-direction * kSpriteScale, kSpriteScale)
		.withOrigin(rect.w / 2, rect.h / 2);

	// Debug-only
	pos.DebugDraw();
	Bounds().DebugDraw();
	AttackBounds().DebugDraw(255, 0, 255);
	BoxBounds::FromCenter(pos, kGroundCollision).DebugDraw();
}
