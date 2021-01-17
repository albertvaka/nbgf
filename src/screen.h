#pragma once

#include "bounds.h"
#include "singleinstance.h"
#include <vector>

struct ScreenManager : SingleInstance<ScreenManager>
{

	void AddScreen(const BoxBounds& b) {
		screens.push_back(b);
	}

	int ScreenCount() {
		return screens.size();
	}

	void DeleteAllScreens() {
		screens.clear();
	}

	int FindScreenContaining(vec pos) const {
		int i = 0;
		for (const auto& screen : screens) {
			if (screen.Contains(pos)) {
				return i;
			}
			i++;
		}
		return -1;
	}

	int CurrentScreen() const {
		return currentScreen;
	}

	const BoxBounds& CurrentBounds() const {
		return screens[currentScreen];
	}

	const BoxBounds& ScreenBounds(int screen) const {
		return screens[screen];
	}

	const void ClampCameraToScreen(vec& camPos) const;

	void UpdateCurrentScreen(vec pos);

private:
	int currentScreen = -1;
	std::vector<BoxBounds> screens;

};

