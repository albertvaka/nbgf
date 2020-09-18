#include "bipedal.h"

#include "jumpman.h"
#include "screen.h"
#include "assets.h"
#include "window.h"
#include "rand.h"
#include "debug.h"
#include "fxmanager.h"
#include "camera.h"

const float walking_speed = 30.f;
const float maxDistance = 200.f;

Bipedal::Bipedal(const vec& pos)
	: BoxEntity(pos + vec(8.f, -2.f), vec(20,10))
	, state(State::WALKING_FORWARD)
	, anim(AnimLib::BIPEDAL_WALKING)
{
	initialX = pos.x;
	screen = ScreenManager::instance()->FindScreenContaining(pos);
}

inline bool Bipedal::inSameScreenAsPlayer() const {
	return screen == -1 || screen == ScreenManager::instance()->CurrentScreen();
}

void Bipedal::Update(float dt)
{
	switch (state) {
	case State::DRAMATIC_PAUSE:
		timer -= dt;
		if (timer < 0) {
			bool forward;
			if (pos.x < initialX) {
				forward = true;
			}
			else if (pos.x > (initialX + maxDistance)) {
				forward = false;
			}
			else {
				forward = Rand::OnceEach(2);
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
			anim.Reverse(dt);
			speed = -walking_speed;
		}

		pos.x += speed * dt;

		if (Camera::GetBounds().Contains(pos)) {
			bool stomp = (frame != anim.current_frame) && (anim.current_frame == 0 || anim.current_frame == 3);
			if (stomp) {
				FxManager::StartScreenshakePreset(charging? FxManager::Stomp : FxManager::LittleStomp);
			}
		}

		bool outOfBounds = (pos.x < initialX) || (pos.x > (initialX + maxDistance));
		bool animationLooped = (frame != anim.current_frame) && (anim.current_frame == 0);
		if (animationLooped && (outOfBounds || Rand::OnceEach(3))) {
			state = State::DRAMATIC_PAUSE;
			timer = Rand::rollf(0.5f, 1.0f);
		}
		break;
	}
}

void Bipedal::Draw() const
{
	Window::Draw(Assets::scifiTexture, pos)
		.withScale(2.f)
		.withOrigin(16.f, 14.f)
		.withRect(anim.GetCurrentRect());

	if (Debug::Draw && Camera::GetBounds().Contains(pos)) {
		DrawBounds();
		Bounds(initialX, pos.y, maxDistance, 2).Draw(255, 255, 0);
	}
	pos.Debuggerino();

}

