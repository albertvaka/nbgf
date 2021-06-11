
#include "ChainNode.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "camera.h"
#include "window.h"

constexpr uint8_t NodeSpeed = 500;
constexpr uint8_t NodeRadius = 50;

ChainNode::ChainNode(vec aPosition)
	: CircleEntity(aPosition, NodeRadius)
{
}

void ChainNode::Update(float aDt)
{
	pos.x -= Input::IsPressed(0, GameKeys::LEFT) * NodeSpeed * aDt;
	pos.x += Input::IsPressed(0, GameKeys::RIGHT) * NodeSpeed * aDt;
	pos.y += Input::IsPressed(0, GameKeys::DOWN) * NodeSpeed * aDt;
	pos.y -= Input::IsPressed(0, GameKeys::UP) * NodeSpeed * aDt;

}

void ChainNode::Draw() const
{
	const GPU_Rect& animRect = AnimLib::PLAYER;
	Window::Draw(Assets::invadersTexture, pos)
		.withRect(animRect)
		.withOrigin(vec(animRect.w, 0) / 2);
}
