#include "fireslime.h"

#include "player.h"
#include "collide.h"
#include "window_draw.h"
#include "assets.h"
#include "rand.h"
#include "fireshot.h"
#include "screen.h"
#include "assets_sounds.h"
#include "common_enemy.h"
#include "common_tilemapcharacter.h"
#include "enemies_by_screen.h"

constexpr float kSpeed = 25;

constexpr vec kShotSpeed(70, -90);
constexpr vec kShotAccel(0, 150);

constexpr float kSpriteScale = 1.3f;
constexpr float kSpriteOffsetY = 7.f;

constexpr int kFirstFrameOnAir = 2;
constexpr int kFirstFrameOnGround = 6;
constexpr int kFrameShooting = 2;

constexpr float kShotsPerAttack = 4;
constexpr float kAttackFrameDuration = AnimLib::FIRESLIME_ATTACK[kFrameShooting].duration;
constexpr float kTimeBetweenShots = kAttackFrameDuration / kShotsPerAttack;

constexpr vec kGroundCollision = vec(10, 10) * kSpriteScale;
constexpr int kFramesOnAir = kFirstFrameOnGround - kFirstFrameOnAir;
constexpr float kBigDt = Animation::TotalDurationForFrames(AnimLib::FIRESLIME_WALK, kFirstFrameOnAir, kFramesOnAir);


FireSlime::FireSlime(vec pos)
	: CircleEntity(pos - vec(0, kSpriteOffsetY), 5*kSpriteScale)
	, anim(AnimLib::FIRESLIME_WALK, false)
	, screen(ScreenManager::FindScreenContaining(pos))
{
	direction = Rand::OnceEvery(2) ? 1 : -1;

	this->pos = AlignWithGround(this->pos, kGroundCollision);
	EnemiesByScreen::Add(screen, this);
}

FireSlime::~FireSlime()
{
	EnemiesByScreen::Remove(screen, this);
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
	vec vel = vec(kSpeed * direction, 0);
	return !IsGoingToHitAWall(pos, kGroundCollision, vel, kBigDt) && !IsGoingToRunOffPlatform(pos, kGroundCollision, vel, kBigDt);
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
				Assets::soundSlimyAttack.Play(pos, Player::instance()->pos);
			}
		}
		break;
	case State::WALKING:
		willAttack = willAttack || (!didJustAttack && Collide(AttackBounds(), Player::instance()->HitBoxBounds()));
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
		Assets::soundSlimyDead.Play(pos, Player::instance()->pos);
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
