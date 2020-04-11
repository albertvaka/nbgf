#pragma once

#include <SFML/Graphics.hpp>

#include "bounds.h"
#include "singleinstance.h"
#include <vector>

struct ScreenManager : SingleInstance<ScreenManager>
{
	int CurrentScreen = 0;
	std::vector<Bounds> screens;

	ScreenManager();

	int FindScreen(const vec& pos) {
		int i = 0;
		for (const auto& screen : screens) {
			if (screen.contains(pos)) {
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

	void UpdateCurrentScreen(const vec& pos);
};

