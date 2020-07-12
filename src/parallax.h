#pragma once

#include "selfregister.h"
#include "bounds.h"

struct Parallax : SelfRegister<Parallax>
{
	Parallax(const Bounds& bounds) : bounds(bounds) { }

	Bounds bounds;

	void Draw() const;

};

