
#include "player.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "camera.h"
#include "window.h"

constexpr uint8_t PlayerSpeed = 500;

Player::Player(float angle, float distance)
	: distance(distance)
{
}

void Player::Update(float aDt)
{
	pos.x -= Input::IsPressed(0, GameKeys::LEFT) * PlayerSpeed * aDt;
	pos.x += Input::IsPressed(0, GameKeys::RIGHT) * PlayerSpeed * aDt;
	pos.y += Input::IsPressed(0, GameKeys::DOWN) * PlayerSpeed * aDt;
	pos.y -= Input::IsPressed(0, GameKeys::UP) * PlayerSpeed * aDt;

}

void Player::Draw() const
{
	const GPU_Rect& animRect = AnimLib::PLAYER;
	Window::Draw(Assets::invadersTexture, pos)
		.withRect(animRect)
		.withOrigin(vec(animRect.w, 0)/2)
		.withRotationDegs(Camera::Center().AngleDegs(pos) + 90);
}
