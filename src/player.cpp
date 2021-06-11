
#include "player.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "camera.h"
#include "window.h"


const float bulletSpeed = 200.f;

Player::Player(float angle, float distance)
	: distance(distance)
{
}

void Player::Update(float dt)
{
	vec mouseFromCenter = Mouse::GetPositionInWorld() - Camera::Center();
	pos = Camera::Center() + mouseFromCenter.Normalized() * distance;
	
}

void Player::Draw() const
{
	const GPU_Rect& animRect = AnimLib::PLAYER;
	Window::Draw(Assets::invadersTexture, pos)
		.withRect(animRect)
		.withOrigin(vec(animRect.w, 0)/2)
		.withRotationDegs(Camera::Center().AngleDegs(pos) + 90);
}
