#include "EnvironmentObject.h"
#include "assets.h"

EnvironmentObject::EnvironmentObject(vec pos) : BoxEntity(pos, vec(100, 100))
{
}

EnvironmentObject::~EnvironmentObject()
{
}

void EnvironmentObject::OnCollision()
{
	//Add default collision
}

std::pair<Window::PartialDraw, Window::PartialDraw> EnvironmentObject::PartialDraws()
{
	Window::PartialDraw shadow = Window::PartialDraw(Assets::lightShadowTexture, pos)
		.withOrigin(Assets::lightShadowTexture->w, Assets::lightShadowTexture->h);
	Window::PartialDraw sprite = Window::PartialDraw(Assets::lightTexture, pos)
		.withOrigin(Assets::lightTexture->w / 2, Assets::lightTexture->h);
	std::pair<Window::PartialDraw, Window::PartialDraw> p(shadow, sprite);
	return p;
}

Window::PartialDraw EnvironmentObject::PartialDraw()
{
	return Window::PartialDraw(Assets::charcoTexture, pos)
		.withScale(0, 0);
}
