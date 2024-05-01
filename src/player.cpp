
#include "player.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "bullet.h"
#include "window.h"


const float bulletSpeed = 200.f;

Player::Player(float distance)
	: distance(distance)
{
}

void Player::Update(float dt)
{
	vec mouseFromCenter = GetMousePositionInWorld() - GameCamera::Center();
	pos = GameCamera::Center() + mouseFromCenter.Normalized() * distance;
	
	if (Input::IsJustPressed(0, GameKeys::SHOOT)) {
		new Bullet(pos, mouseFromCenter.Normalized() * bulletSpeed);
	}
	
}

void Player::Draw() const
{
	const Rectangle& animRect = AnimLib::PLAYER;
	Window::Draw(Assets::invadersTexture, pos)
		.withRect(animRect)
		.withOrigin(vec(animRect.width, 0)/2)
		.withRotationDegs(GameCamera::Center().AngleDegs(pos) + 90);
}
