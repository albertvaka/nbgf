
#include "player.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "bullet.h"
#include "rand.h"
#include "window.h"


const float kBulletPeriod = 0.18f;
const float kBulletSpeed = 450.f;
const float kFastSpeed = 280.f;
const float kSlowSpeed = 350.f/4;

Player::Player()
	: CircleEntity(5)
	, playerNum(0)
{
}

void Player::Reset() {
	pos = vec(0.5f, 0.9f) * Camera::Size();
	shotTimer = 0.f;
	alive = true;
}

void Player::Update(float dt)
{
	vec dir = Input::GetAnalog(playerNum, AnalogInput::MOVE);
	float speed = Input::IsPressed(playerNum, GameKeys::SLOWDOWN) ? kSlowSpeed : kFastSpeed;
	pos += dir * speed * dt;
	
	shotTimer -= dt;
	if (Input::IsPressed(playerNum, GameKeys::SHOOT) && shotTimer <= 0.f){
		shotTimer = kBulletPeriod;
		new Bullet(pos, vec(0,-kBulletSpeed));
		if (Rand::OnceEvery(2)) {
			Assets::shootSound.Play();
		} 
		else {
			Assets::shootSound2.Play();
		}
	}

	pos = Camera::Bounds().ClosestPointInBounds(pos);
}

void Player::Draw() const
{
	if (!alive) return;

	const GPU_Rect& animRect = AnimLib::PLAYER;
	Window::Draw(Assets::spritesTexture, pos)
		.withRect(animRect)
		.withScale(vec(2.0f))
		.withOrigin(vec(animRect.w, animRect.h+4) / 2);

	Bounds().DebugDraw();
}
