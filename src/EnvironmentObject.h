#pragma once
#include "entity.h"

#include "window.h"

enum class CollisionState {
	Outside,
	Inside
};

struct EnvironmentObject : public BoxEntity
{
	EnvironmentObject(vec pos);
	virtual ~EnvironmentObject();

	virtual void OnCollision();
	virtual void Draw() {}
	virtual std::pair<Window::PartialDraw, Window::PartialDraw> PartialDraws();
	virtual Window::PartialDraw PartialDraw();

	CollisionState previous = CollisionState::Outside;
};

