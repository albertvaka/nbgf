#include "debug_walker.h"

#include "mates.h"
#include "assets.h"
#include "window.h"
#include "common_tilemapcharacter.h"
#include "debug.h"

// accel
const float gravity_acc = 600;

// friction Y
const float fri_acc_wall_up = 1200;
const float fri_acc_wall_down = 450;

// jump
const float vel_jump = -150;
const float jump_time = 0.35f;

// Sprite
const vec center = vec(8, 16);
 
DebugWalker::DebugWalker(vec position, float velX)
	: animation(AnimLib::MARIO_IDLE)
{
	pos = position;
	vel.x = velX;
}

void DebugWalker::Jump() {
	if (jumpTimeLeft <= 0) {
		jumpTimeLeft = jump_time;
	}
}

void DebugWalker::Update(float dt)
{
	if (!alive) return;

	grounded = IsGrounded(pos, size);


	if (jumpTimeLeft > 0)
	{
		vel.y = vel_jump;
	}
	else
	{
		jumpTimeLeft = 0; // Stop jumping
	}

	if (jumpTimeLeft > 0)
	{
		jumpTimeLeft -= dt; //We are still jumping
	}

	vec fri = vec(0, 0);
	vel.y += gravity_acc * dt;

	if (onWall)
	{
		if (vel.y < 0) { // Moving up
			fri.y = fri_acc_wall_up;
		}
		else {
			fri.y = fri_acc_wall_down;
		}
	}

	if (fri.y != 0)
	{
		if (vel.y < 0)
		{
			vel.y += fri.y * dt;
			if (vel.y > 0) vel.y = 0;
		}
		else if (vel.y > 0)
		{
			vel.y -= fri.y * dt;
			if (vel.y < 0) vel.y = 0;
		}

	}

	MoveResult moved = MoveAgainstTileMap(pos, size, vel, dt);
	pos = moved.pos;

	if (moved.leftWallCollision != Tile::NONE) {
		onWall = ONWALL_LEFT;
	} else if (moved.rightWallCollision != Tile::NONE) {
		onWall = ONWALL_RIGHT;
	} else {
		onWall = ONWALL_NO;
	}

	lookingLeft = vel.x < 0;

	if (moved.ceilingCollision != Tile::NONE) {
		vel.y = 0;
		jumpTimeLeft = 0;
	}
	if (moved.groundCollision != Tile::NONE) {
		if (moved.groundCollision.isSlope()) {
			vel.y = 30.f; //this helps you get grounded as soon as the slope ends
		}
		else {
			vel.y = 0;
		}
		onWall = ONWALL_NO;
		grounded = true;
	}

	animation.Update(dt);

	if (grounded)
	{
		if (vel.x < 0 || vel.x > 0)
		{
			animation.Ensure(AnimLib::MARIO_WALK);
		}
		else
		{
			animation.Ensure(AnimLib::MARIO_IDLE);
		}
	}
	else
	{
		if (onWall) animation.Ensure(AnimLib::MARIO_ONWALL);
		else animation.Ensure(AnimLib::MARIO_JUMP);
	}
}

void DebugWalker::Draw() const {

	Window::Draw(Assets::marioTexture, pos)
		.withOrigin(center.x, center.y)
		.withRect(animation.CurrentFrameRect())
		.withScale(lookingLeft ? -1.f : 1.f, 1.f);

	// Debug-only
	Bounds().DebugDraw();
}


