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
#include "common_enemy.h"

const float walking_speed = 30.f; //per second

// Constants relatives to the sprite, assuming the origin is at ground level
const vec headHitBoxOffset = vec(-30, -105);
const vec headHitBoxSize = vec(80, 45);
const vec legsHitBoxOffset = vec(-20, -64);
const vec legsHitBoxSize = vec(40, 60);
const vec textureOffset = vec(-10, -88); 
const vec missilesOriginOffset = vec(-10, -110);
const float legsReceiveHitWidthIncrease = 20.f;

Bipedal::Bipedal(vec pos)
	: Entity(pos)
	, timer(0)
	, anim(AnimLib::BIPEDAL_WALKING)
	, state(State::WALKING_FORWARD)
	, headHitBox(pos + headHitBoxOffset, headHitBoxSize)
	, legsHitBox(pos + legsHitBoxOffset, legsHitBoxSize)
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
	if (!InSameScreenAsPlayer(screen)) {
		return;
	}

	hitTimer -= dt;

	const vec* damageFromPlayerPos = ReceiveDamageFromPlayer(headHitBox, hitTimer > 0.f);
	if (damageFromPlayerPos) {
		TakeDamage();
		if (alive == false) return;
	} else if (ReceiveDamageFromPlayer(legsHitBox.Grown(legsReceiveHitWidthIncrease,0), hitTimer > 0.f)) {
		// Ignore attacks to the legs. We still call ReceiveDamageFromPlayer so the player gets the knockback. On an else to not knock back twice
		// TODO: We could play a sfx here.
	}

	switch (state) {
	case State::DEACTIVATED:
		return;
	case State::FIRING:
	{
		float oldTimer = timer;
		float timeBetweenMissiles = 0.4f;
		vec shotsOrigin = pos + missilesOriginOffset;
		timer += dt;
		if (oldTimer < timeBetweenMissiles && timer >= timeBetweenMissiles) {
			new Missile(shotsOrigin, -135);
		}
		else if (oldTimer < 2 * timeBetweenMissiles && timer >= 2 * timeBetweenMissiles) {
			new Missile(shotsOrigin, -90);
		}
		else if (oldTimer < 3 * timeBetweenMissiles && timer >= 3 * timeBetweenMissiles) {
			new Missile(shotsOrigin, -45);
		}
		else if (timer >= 4 * timeBetweenMissiles) {
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
		headHitBox.left = pos.x + headHitBoxOffset.x;
		legsHitBox.left = pos.x + legsHitBoxOffset.x;

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

void Bipedal::Die() {
	alive = false;
	DieScreenShake();
	for (int i = 0; i < 6; i++) {
		RandomlySpawnHealth(Rand::VecInRange(headHitBox), 100);
	}
}

void Bipedal::TakeDamage() {
	hitTimer = 0.3f;
	health--;
	if (health <= 0) {
		Die();
	}
}

void Bipedal::Draw() const
{

	if (hitTimer > 0.f) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
	}

	Window::Draw(Assets::scifiTexture, pos+textureOffset)
		.withScale(2.f)
		.withOrigin(16.f, 14.f)
		.withRect(anim.CurrentFrameRect());

	Shader::Deactivate();

	// Debug-only
	legsHitBox.DebugDraw();
	headHitBox.DebugDraw();
	legsHitBox.Grown(legsReceiveHitWidthIncrease, 0).DebugDraw(0, 255, 0);
	BoxBounds(Tiled::Areas::boss_bounds[0].Left(), pos.y, Tiled::Areas::boss_bounds[0].width, 2).DebugDraw(255, 255, 0);
	BoxBounds(minX, pos.y, maxX - minX, 2).DebugDraw(0, 255, 0);
	pos.DebugDraw();

}

