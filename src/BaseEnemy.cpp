#include "BaseEnemy.h"

#include "anim_lib.h"
#include "assets.h"
#include "window.h"
#include "camera.h"
#include "rand.h"

BaseEnemy::BaseEnemy(float angle, float distance)
{
	SetRandomSpawn();
}

void BaseEnemy::Update(float dt)
{
	pos += vel * speed * dt;
}

void BaseEnemy::Draw() const
{
	const GPU_Rect& animRect = AnimLib::PLAYER;
	Window::Draw(Assets::invadersTexture, pos)
		.withRect(animRect)
		.withOrigin(vec(animRect.w, 0) / 2)
		.withRotationDegs(Camera::Center().AngleDegs(pos) + 90);
}

void BaseEnemy::SetRandomSpawn()
{
	SetRandomSpawnSide();

	SetSpawnPosition();
	
	SetSpawnSpeed();
}

void BaseEnemy::SetRandomSpawnSide()
{
	float random = Rand::rollf(0.0f, 1.0f);

	if (random < 0.25f) {
		spawnSide = SpawnSide::Top;
	}
	else if (random >= 0.25f && random < 0.5f) {
		spawnSide = SpawnSide::Down;
	}
	else if (random >= 0.5f && random < 0.75f) {
		spawnSide = SpawnSide::Left;
	}
	else {
		spawnSide = SpawnSide::Right;
	}

}

void BaseEnemy::SetSpawnPosition()
{
	float random = Rand::rollf(0.1f, 0.9f);
	switch (spawnSide)
	{
	case SpawnSide::Top:
		pos.y = 0;
		pos.x = random * Window::GAME_WIDTH;
		break;
	case SpawnSide::Down:
		pos.y = Window::GAME_HEIGHT;
		pos.x = random * Window::GAME_WIDTH;
		break;
	case SpawnSide::Right:
		pos.x = Window::GAME_WIDTH;
		pos.y = random * Window::GAME_HEIGHT;
		break;
	case SpawnSide::Left:
		pos.x = 0;
		pos.y = random * Window::GAME_HEIGHT;
		break;
	}
}

void BaseEnemy::SetSpawnSpeed()
{
	switch (spawnSide)
	{
	case SpawnSide::Top:
		vel.y = 1.0f;
		vel.x = 0;
		break;
	case SpawnSide::Down:
		vel.y = -1.0f;
		vel.x = 0;
		break;
	case SpawnSide::Right:
		vel.x = -1.0f;
		vel.y = 0;
		break;
	case SpawnSide::Left:
		vel.x = 1.0f;
		vel.y = 0;
		break;
	}
}

float BaseEnemy::Clamp(float value, float edge1, float edge2) {
	if (value < edge1) {
		return edge1;
	}
	else if (value > edge2) {
		return edge2;
	}
	else {
		return value;
	}
}
