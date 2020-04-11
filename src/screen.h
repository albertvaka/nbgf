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

	int FindScreen(const vec& pos) const {
		int i = 0;
		for (const auto& screen : screens) {
			if (screen.contains(pos)) {
				return i;
			}
			i++;
		}
		return -1;
	}

	const Bounds& CurrentBounds() const {
		return screens[CurrentScreen];
	}

	const Bounds& ScreenBounds(int screen) const {
		return screens[screen];
	}

	const void ClampCameraToScreen(vec& camPos) const;

	void UpdateCurrentScreen(const vec& pos);
};

