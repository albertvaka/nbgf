#pragma once

#include "screen.h"
#include "bounds.h"
#include "tiled_objects_zones.h"

struct ZoneManager {

	const SDL_Color normalBg = { 31, 36, 50, 255 };
	const SDL_Color caveBg = { 18, 25, 26, 255 };
	const SDL_Color pinkCaveBg = { 59, 17, 88, 255 };
	const SDL_Color sewerBg = { 0, 0, 0, 255 };
	const float maxChangeDeltaPerSecond = 150;

	SDL_Color currentBgColor;
	SDL_Color targetBgColor;
	int currentScreen;


	void Reset() {
		currentScreen = ScreenManager::instance()->CurrentScreen();
		currentBgColor = targetBgColor = FindColorForScreen(currentScreen);
	}

	void UpdateColor(uint8_t& current, uint8_t target, float dt) {
		if (current != target) {
			uint8_t delta = maxChangeDeltaPerSecond * dt;
			if (delta == 0) delta = 1;
			if (target > current) {
				uint8_t maxDelta = target - current;
				if (delta > maxDelta) {
					delta = maxDelta;
				}
				current += delta;
			}
			else {
				uint8_t maxDelta = current - target;
				if (delta > maxDelta) {
					delta = maxDelta;
				}
				current -= delta;
			}

		}
	}

	void Update(float dt) {
		int actualCurrentScreen = ScreenManager::instance()->CurrentScreen();
		if (currentScreen != actualCurrentScreen) {
			currentScreen = actualCurrentScreen;
			targetBgColor = FindColorForScreen(currentScreen);
		}
		UpdateColor(currentBgColor.r, targetBgColor.r, dt);
		UpdateColor(currentBgColor.g, targetBgColor.g, dt);
		UpdateColor(currentBgColor.b, targetBgColor.b, dt);
	}

	const SDL_Color& GetBgColor() {
		return currentBgColor;
	}

	const SDL_Color& FindColorForScreen(int screen) {
		BoxBounds screenBounds = ScreenManager::instance()->ScreenBounds(screen);
		for (const BoxBounds& b : Tiled::Zones::cave) {
			if (b.Contains(screenBounds.Center())) {
				return caveBg;
			}
		}
		for (const BoxBounds& b : Tiled::Zones::pink_cave) {
			if (b.Contains(screenBounds.Center())) {
				return pinkCaveBg;
			}
		}
		for (const BoxBounds& b : Tiled::Zones::sewers) {
			if (b.Contains(screenBounds.Center())) {
				return sewerBg;
			}
		}
		return normalBg;
	}
};