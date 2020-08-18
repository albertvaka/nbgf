#pragma once

#include "bounds.h"
#include "singleinstance.h"
#include <vector>

struct ScreenManager : SingleInstance<ScreenManager>
{

	void AddScreen(const Bounds& b) {
		screens.push_back(b);
	}

	int ScreenCount() {
		return screens.size();
	}

	void ClearScreenList() {
		screens.clear();
	}

	int FindScreenContaining(const vec& pos) const {
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

	const Bounds& CurrentBounds() const {
		return screens[currentScreen];
	}

	const Bounds& ScreenBounds(int screen) const {
		return screens[screen];
	}

	const void ClampCameraToScreen(vec& camPos) const;

	void UpdateCurrentScreen(const vec& pos);

private:
	int currentScreen = -1;
	std::vector<Bounds> screens;

};

