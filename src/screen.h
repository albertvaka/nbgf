#pragma once

#include <SFML/Graphics.hpp>

#include "bounds.h"
#include "entity.h"
#include "singleinstance.h"
#include <vector>

struct ScreenManager : SingleInstance<ScreenManager>
{
	int CurrentScreen = 0;
	std::vector<Bounds> screens;

	ScreenManager();

	int FindScreen(const Entity* e) {
		int i = 0;
		for (const auto& screen : screens) {
			if (screen.contains(e->pos)) {
				return i;
			}
			i++;
		}
		return CurrentScreen;
	}

	const Bounds& CurrentBounds() {
		return screens[CurrentScreen];
	}

	const void ClampCameraToScreen(vec& camPos);

	void Update(float dt);
};

