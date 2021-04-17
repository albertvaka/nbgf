
#include "player.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "bullet.h"
#include "window.h"


const float kBulletSpeed = 500.f;
const float kSpeed = 350.f;

Player::Player(vec pos)
	: CircleEntity(pos, 8)
	, playerNum(0)
{
}

void Player::Update(float dt)
{
	vec dir = Input::GetAnalog(playerNum, AnalogInput::MOVE).Normalized();
	pos += dir * kSpeed * dt;
	
	if (Input::IsJustPressed(playerNum, GameKeys::SHOOT)){
		new Bullet(pos, vec(0,-kBulletSpeed));
	}
}

void Player::Draw() const
{
	const GPU_Rect& animRect = AnimLib::PLAYER;
	Window::Draw(Assets::invadersTexture, pos)
		.withRect(animRect)
		.withOrigin(vec(animRect.w, animRect.h) / 2);

	Bounds().DebugDraw();
}
