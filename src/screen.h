#pragma once

#include "bounds.h"
#include "singleinstance.h"
#include "window_conf.h"
#include "entity.h"
#include "debug.h"

#include <SDL_assert.h>

#include <vector>

struct ScreenManager
{

	static int AddScreen(const BoxBounds& b) {
		screens.push_back(b);
		return screens.size()-1;
	}

	static int ScreenCount() {
		return screens.size();
	}

	static void DeleteAllScreens() {
		currentScreen = -1;
		screens.clear();
	}

	static int FindScreenContaining(vec pos);

	static int CurrentScreen() {
		return currentScreen;
	}

	static const BoxBounds& CurrentBounds() {
		return screens[currentScreen];
	}

	static const BoxBounds& ScreenBounds(int screen) {
		SDL_assert(screen >= 0 && screen < screens.size());
		return screens[screen];
	}

	static void ClampCameraToScreen(vec& camPos);

	static void UpdateCurrentScreen(vec pos);

private:
	static int currentScreen;
	static std::vector<BoxBounds> screens;
};

inline bool InSameScreenAsPlayer(int myScreen) {
	return myScreen == -1 || myScreen == ScreenManager::CurrentScreen();
}

