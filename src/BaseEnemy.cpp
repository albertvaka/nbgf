#include "BaseEnemy.h"

#include "anim_lib.h"
#include "assets.h"
#include "window.h"
#include "camera.h"
#include "rand.h"

BaseEnemy::BaseEnemy(float angle, float distance)
{
	SetRandomSpawnPosition();
}

void BaseEnemy::Update(float dt)
{
	pos.y += 500 * dt;
}

void BaseEnemy::Draw() const
{
	const GPU_Rect& animRect = AnimLib::PLAYER;
	Window::Draw(Assets::invadersTexture, pos)
		.withRect(animRect)
		.withOrigin(vec(animRect.w, 0) / 2)
		.withRotationDegs(Camera::Center().AngleDegs(pos) + 90);
}

void BaseEnemy::SetRandomSpawnPosition()
{
	float random = Rand::rollf(Window::GAME_WIDTH * 0.1f, Window::GAME_WIDTH * 0.9f);
	pos.x = random;
	pos.y = 0;
}
