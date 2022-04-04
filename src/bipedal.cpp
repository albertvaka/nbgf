#include "bipedal.h"

#include "screen.h"
#include "assets.h"
#include "window_draw.h"
#include "rand.h"
#include "debug.h"
#include "shader.h"
#include "tiled_objects_areas.h"
#include "missile.h"
#include "fx.h"
#include "camera.h"
#include "oneshotanim.h"
#include "common_enemy.h"

const float walking_speed = 30.f; //per second

// Constants relatives to the sprite, assuming the origin is at ground level
const vec kHeadHitBoxOffset = vec(-30, -105);
const vec kHeadHitBoxSize = vec(80, 45);
const vec kLegsHitBoxOffset = vec(-20, -64);
const vec kLegsHitBoxSize = vec(40, 60);
const vec kTextureOffset = vec(-10, -88);
const vec kMissilesOriginOffset = vec(-10, -110);
const float kLegsReceiveHitWidthIncrease = 20.f;
const float kTimeBetweenMissiles = 0.4f;
const float kDieAnimTime = 3.f;
const int kHealth = 18;

Bipedal::Bipedal(vec pos)
	: Entity(pos)
	, health(kHealth)
	, anim(AnimLib::BIPEDAL_WALKING)
	, state(State::WALKING_FORWARD)
	, headHitBox(pos + kHeadHitBoxOffset, kHeadHitBoxSize)
	, legsHitBox(pos + kLegsHitBoxOffset, kLegsHitBoxSize)
{
	int bounds_index = FindIndexOfSmallestBoundsContaining(pos, Tiled::Areas::boss_bounds);
	if (bounds_index >= Tiled::Areas::boss_bounds.size()) {
		Debug::out << "Bipedal boss outside boss_bounds";
		minX = pos.x;
		maxX = pos.x;
	} else {
		minX = Tiled::Areas::boss_bounds[bounds_index].Left();
		maxX = Tiled::Areas::boss_bounds[bounds_index].Right();

		float maxDistanceItCanWalkInOneLoop = anim.TotalDuration() * walking_speed;
		minX += maxDistanceItCanWalkInOneLoop; // We do this since it never changes direction mid-animation
		maxX -= maxDistanceItCanWalkInOneLoop;
	}
	screen = ScreenManager::instance()->FindScreenContaining(pos);
}

void Bipedal::Update(float dt)
{
	if (state == State::DYING) {
		timer -= dt;
		hitTimer = hitTimer > 0 ? 0.f : 1.f;

		if (Rand::OnceEvery(15)) {
			vec explosionPos = Rand::OnceEvery(2) ? Rand::VecInRange(headHitBox) : Rand::VecInRange(legsHitBox);
			float rand = Rand::rollf();
			if (rand > 0.666f) {
				new ForegroundOneShotAnim(Assets::scifiTexture, explosionPos, AnimLib::EXPLOSION_1, 0.75f);
			} else if (rand > 0.333f) {
				new ForegroundOneShotAnim(Assets::scifiTexture, explosionPos, AnimLib::EXPLOSION_2, 0.75f);
			} else {
				new ForegroundOneShotAnim(Assets::scifiTexture, explosionPos, AnimLib::EXPLOSION_3, 0.75f);
			}
		}
		if (timer < 0) {
			new ForegroundOneShotAnim(Assets::scifiTexture, headHitBox.Center(), AnimLib::EXPLOSION_3, 0.75f);
			alive = false;
			for (int i = 0; i < 6; i++) {
				RandomlySpawnHealth(Rand::VecInRange(headHitBox), 100);
			}
		}
		return;
	}

	if (!InSameScreenAsPlayer(screen)) {
		return;
	}

	hitTimer -= dt;

	const vec* damageFromPlayerPos = ReceiveDamageFromPlayer(headHitBox, hitTimer > 0.f);
	if (damageFromPlayerPos) {
		TakeDamage();
		if (alive == false) return;
	} else if (ReceiveDamageFromPlayer(legsHitBox.Grown(kLegsReceiveHitWidthIncrease,0), hitTimer > 0.f)) {
		// Ignore attacks to the legs. We still call ReceiveDamageFromPlayer so the player gets the knockback. On an else to not knock back twice
		// TODO: We could play a sfx here.
	}

	switch (state) {
	case State::DYING:
	case State::DEACTIVATED:
		return;
	case State::FIRING:
	{
		float oldTimer = timer;
		vec shotsOrigin = pos + kMissilesOriginOffset;
		timer += dt;
		if (oldTimer < kTimeBetweenMissiles && timer >= kTimeBetweenMissiles) {
			new Missile(shotsOrigin, -135);
		}
		else if (oldTimer < 2 * kTimeBetweenMissiles && timer >= 2 * kTimeBetweenMissiles) {
			new Missile(shotsOrigin, -90);
		}
		else if (oldTimer < 3 * kTimeBetweenMissiles && timer >= 3 * kTimeBetweenMissiles) {
			new Missile(shotsOrigin, -45);
		}
		else if (timer >= 4 * kTimeBetweenMissiles) {
			timer = 0.5f;
			state = State::DRAMATIC_PAUSE;
		}
	}
		break;
	case State::DRAMATIC_PAUSE:
		timer -= dt;
		if (timer < 0) {
			bool forward;
			if (pos.x < minX) {
				forward = true;
			}
			else if (pos.x > (maxX)) {
				forward = false;
			}
			else {
				forward = Rand::OnceEvery(2);
			}
			if (forward) {
				state = State::WALKING_FORWARD;
				anim.current_frame = 1;
			}
			else {
				state = State::WALKING_BACK;
				anim.current_frame = anim.anim_size - 1;
			}
			charging = Rand::rollf() < 0.45f;
		}
		break;
	case State::WALKING_FORWARD: //fall-through
	case State::WALKING_BACK:

		if (charging) {
			dt *= 2;
		}

		int frame = anim.current_frame;

		float speed;

		if (state == State::WALKING_FORWARD) {
			anim.Update(dt);
			speed = walking_speed;
		}
		else {
			anim.UpdateReverse(dt);
			speed = -walking_speed;
		}

		pos.x += speed * dt;
		headHitBox.left = pos.x + kHeadHitBoxOffset.x;
		legsHitBox.left = pos.x + kLegsHitBoxOffset.x;

		if (Camera::Bounds().Contains(pos)) {
			bool stomp = (frame != anim.current_frame) && (anim.current_frame == 0 || anim.current_frame == 3);
			if (stomp) {
				Fx::Screenshake::StartPreset(charging? Fx::Screenshake::Preset::Stomp : Fx::Screenshake::Preset::LittleStomp);
			}
		}

		bool outOfBounds = (pos.x < minX) || (pos.x > maxX);
		bool animationLooped = (frame != anim.current_frame) && (anim.current_frame == 0);
		if (animationLooped && (outOfBounds || Rand::PercentChance(40))) {
			if (Rand::OnceEvery(3)) {
				state = State::DRAMATIC_PAUSE;
				timer = Rand::rollf(0.5f, 1.0f);
			}
			else {
				state = State::FIRING;
				timer = 0;
			}
		}
		break;
	}

	if (!damageFromPlayerPos) { //avoid hitting and being hit the same frame
		DamagePlayerOnCollision(headHitBox);
		DamagePlayerOnCollision(legsHitBox);
	}
}

void Bipedal::TakeDamage() {
	hitTimer = 0.3f;
	health--;
	if (health <= 0) {
		for (Missile* m : Missile::GetAll()) {
			m->explode();
		}
		Fx::Screenshake::Start(kDieAnimTime, vec(1.5f, 1.5f), vec(30.f, 39.f));
		state = State::DYING;
		timer = kDieAnimTime;
	}
}

void Bipedal::Draw() const
{
	if (hitTimer > 0.f) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
	}

	Window::Draw(Assets::scifiTexture, pos+kTextureOffset)
		.withScale(2.f)
		.withOrigin(16.f, 14.f)
		.withRect(anim.CurrentFrameRect());

	Shader::Deactivate();

	// Debug-only
	legsHitBox.DebugDraw();
	headHitBox.DebugDraw();
	legsHitBox.Grown(kLegsReceiveHitWidthIncrease, 0).DebugDraw(0, 255, 0);
	BoxBounds(Tiled::Areas::boss_bounds[0].Left(), pos.y, Tiled::Areas::boss_bounds[0].width, 2).DebugDraw(255, 255, 0);
	BoxBounds(minX, pos.y, maxX - minX, 2).DebugDraw(0, 255, 0);
	pos.DebugDraw();

}

int Bipedal::DrawHealth(int offset) const {
	if (!InSameScreenAsPlayer(screen)) {
		return 0;
	}
	DrawBossHealth(health, kHealth, offset);
	return 1;
}
