
#include "player.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "bullet.h"
#include "window.h"


const float kBulletSpeed = 500.f;
const float kSpeed = 350.f;

Player::Player(vec pos)
	: CircleEntity(pos, 5)
	, playerNum(0)
{
}

void Player::Update(float dt)
{
	vec dir = Input::GetAnalog(playerNum, AnalogInput::MOVE).Normalized();
	float speed_mult = Input::IsPressed(playerNum, GameKeys::SLOWDOWN) ? 0.25f : 1.0f;
	pos += dir * kSpeed * dt * speed_mult;;
	
	if (Input::IsJustPressed(playerNum, GameKeys::SHOOT)){
		new Bullet(pos, vec(0,-kBulletSpeed));
	}

	pos = Camera::Bounds().ClosestPointInBounds(pos);
}

void Player::Draw() const
{
	const GPU_Rect& animRect = AnimLib::PLAYER;
	Window::Draw(Assets::spritesTexture, pos)
		.withRect(animRect)
		.withScale(vec(2.0f))
		.withOrigin(vec(animRect.w, animRect.h+4) / 2);

	Bounds().DebugDraw();
}
