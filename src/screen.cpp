#include "screen.h"

#include "camera.h"
#include "mates.h"
#include "tiled_objects_screens.h"
#include "common_enemy.h" //FindIndexOfSmallestBoundsContaining

int ScreenManager::currentScreen = -1;
std::vector<BoxBounds> ScreenManager::screens;

void ScreenManager::UpdateCurrentScreen(vec pos) {
	if (currentScreen < 0 || !Tiled::Screens::screen[currentScreen].Contains(pos)) {
		int screen = FindScreenContaining(pos);
		if (screen >= 0) currentScreen = screen;
	}
}

int ScreenManager::FindScreenContaining(vec pos) {
	return FindIndexOfSmallestBoundsContaining(pos, screens);
}

void ScreenManager::ClampCameraToScreen(vec& camPos) {
	if (currentScreen < 0) return;

	const BoxBounds& screenBounds = CurrentBounds();

	float minY = screenBounds.Top() + (Camera::Size().y / 2.f);
	float maxY = screenBounds.Bottom() - (Camera::Size().y / 2.f);
	if (maxY <= minY) {
		camPos.y = screenBounds.Center().y;
	}
	else {
		Mates::Clamp(camPos.y, minY, maxY);
	}

	float minX = screenBounds.Left() + (Camera::Size().x / 2.f);
	float maxX = screenBounds.Right() - (Camera::Size().x / 2.f);
	if (maxX <= minX) {
		camPos.x = screenBounds.Center().x;
	}
	else {
		Mates::Clamp(camPos.x, minX, maxX);
	}
}
