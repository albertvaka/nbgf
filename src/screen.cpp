#include "screen.h"

#include "camera.h"
#include "mates.h"
#include "tiledexport.h"

void ScreenManager::UpdateCurrentScreen(vec pos) {
	if (currentScreen < 0 || !TiledMap::screens[currentScreen].Contains(pos)) {
		int screen = FindScreenContaining(pos);
		if (screen >= 0) currentScreen = screen;
	}
}

const void ScreenManager::ClampCameraToScreen(vec& camPos) const {
	if (currentScreen < 0) return;

	const BoxBounds& screenBounds = CurrentBounds();

	float minY = screenBounds.Top() + (Camera::Size().y / 2.f);
	float maxY = screenBounds.Bottom() - (Camera::Size().y / 2.f);
	if (maxY <= minY) {
		camPos.y = ScreenManager::CurrentBounds().Center().y;
	}
	else {
		Mates::Clamp(camPos.y, minY, maxY);
	}

	float minX = screenBounds.Left() + (Camera::Size().x / 2.f);
	float maxX = screenBounds.Right() - (Camera::Size().x / 2.f);
	if (maxX <= minX) {
		camPos.x = ScreenManager::CurrentBounds().Center().x;
	}
	else {
		Mates::Clamp(camPos.x, minX, maxX);
	}
}
