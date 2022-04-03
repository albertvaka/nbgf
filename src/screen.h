#pragma once

#include "bounds.h"
#include "singleinstance.h"
#include <vector>

struct ScreenManager : SingleInstance<ScreenManager>
{
	int AddScreen(const BoxBounds& b) {
		screens.push_back(b);
		return screens.size()-1;
	}

	int ScreenCount() {
		return screens.size();
	}

	void DeleteAllScreens() {
		screens.clear();
	}

	int FindScreenContaining(vec pos) const;

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

inline bool InSameScreenAsPlayer(int myScreen) {
	return myScreen == -1 || myScreen == ScreenManager::instance()->CurrentScreen();
}
