#include "screen.h"

#include "input.h"
#include "mates.h"
#include "tiledexport.h"

ScreenManager::ScreenManager() {
	for (const auto& screen : TiledMap::screens) {
		screens.emplace_back(screen.left, screen.top, screen.width, screen.height);
	}
}
void ScreenManager::UpdateCurrentScreen(const vec& pos) {
	if (!TiledMap::screens[CurrentScreen].contains(pos)) {
		int screen = FindScreen(pos);
		if (screen >= 0) CurrentScreen = screen;
	}
}

const void ScreenManager::ClampCameraToScreen(vec& camPos) const {
	const Bounds& screenBounds = CurrentBounds();

	float minY = screenBounds.Top() + (Camera::GetCameraSize().y / 2.f);
	float maxY = screenBounds.Bottom() - (Camera::GetCameraSize().y / 2.f);
	if (maxY <= minY) {
		camPos.y = ScreenManager::CurrentBounds().Center().y;
	}
	else {
		Mates::Clamp(camPos.y, minY, maxY);
	}

	float minX = screenBounds.Left() + (Camera::GetCameraSize().x / 2.f);
	float maxX = screenBounds.Right() - (Camera::GetCameraSize().x / 2.f);
	if (maxX <= minX) {
		camPos.x = ScreenManager::CurrentBounds().Center().x;
	}
	else {
		Mates::Clamp(camPos.x, minX, maxX);
	}
}
