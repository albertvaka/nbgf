
#include "player.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "bullet.h"
#include "window.h"


const float kBulletSpeed = 200.f;

Player::Player(float angle, float distance)
	: CircleEntity(vec::Zero, 8)
	, distance(distance)
{
}

void Player::Update(float dt)
{
	vec mouseFromCenter = Mouse::GetPositionInWorld() - Camera::Center();
	pos = Camera::Center() + mouseFromCenter.Normalized() * distance;
	
	if (Mouse::IsJustPressed(Mouse::Button::Left)) {
		new Bullet(pos, mouseFromCenter.Normalized() * kBulletSpeed);
	}
	
}

void Player::Draw() const
{
	const GPU_Rect& animRect = AnimLib::PLAYER;
	Window::Draw(Assets::invadersTexture, pos)
		.withRect(animRect)
		.withOrigin(vec(animRect.w, animRect.h)/2)
		.withRotationDegs(Camera::Center().AngleDegs(pos) + 90);

	Bounds().DebugDraw();
}
