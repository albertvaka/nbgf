
#include "overlord.h"

#include "input.h"
#include "window.h"
#include "debug.h"
#include "anim_lib.h"


Overlord::Overlord()
{
}

void Overlord::Update(float dt)
{
	cursorPos = Mouse::GetPositionInWorld();
		
}

void Overlord::Draw() const
{
	if(Debug::Draw) {
		vec mouseEnd = cursorPos + vec(-1,-1)*20;
		Window::DrawPrimitive::Arrow(mouseEnd, cursorPos, 2, 10, {0, 255, 255, 255});
	}
	/*
	const GPU_Rect& animRect = AnimLib::Overlord;
	Window::Draw(Assets::overlordTexture, pos)
		.withRect(animRect)
		.withOrigin(vec(animRect.w, 0)/2)
	*/
}
