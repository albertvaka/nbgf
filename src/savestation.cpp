#include "savestation.h"

#include "tilemap.h"
#include "mates.h"
#include "assets.h"
#include "camera.h"
#include "screen.h"
#include "collide.h"
#include "jumpman.h"

extern float mainClock;

std::unordered_map<int, std::vector<SaveStation*>> SaveStation::ByScreen;

SaveStation::SaveStation(int id, vec p) 
	: BoxEntity(p, vec(32, 32)) 
	, id(id)
	, hidden(false)
	, glowing(false)
	, prevFrameInScene(true)
{ 
	screen = ScreenManager::instance()->FindScreenContaining(pos);
	SaveStation::ByScreen[screen].push_back(this);
}

SaveStation::~SaveStation() {
	auto& list = SaveStation::ByScreen[screen];
	list.erase(std::find(list.begin(), list.end(), this));
}

bool SaveStation::Update(float dt) 
{
	if (hidden) {
		hiddenBy.erase(std::remove_if(hiddenBy.begin(), hiddenBy.end(), [](const Entity* e) {
			return !e->alive;
			}), hiddenBy.end());
		if (hiddenBy.empty()) {
			hidden = false;
		} else {
			prevFrameInScene = false;
		}
		return false;
	}

	if (!glowing) {
		bool inScene = (screen == ScreenManager::instance()->CurrentScreen());
		if (inScene && !prevFrameInScene) {
			glowing = true;
		}
		prevFrameInScene = inScene;
		return false;
	}

	return Collide(bounds(), JumpMan::instance()->bounds());
}

void SaveStation::Draw() const
{
	if (hidden) {
		return;
	}

	Window::Draw(Assets::marioTexture, pos)
		.withRect(4 * 16, 12 * 16, 32, 32)
		.withOrigin(16, 16);

	if (glowing) {
		Window::Draw(Assets::marioTexture, pos)
			.withRect(6 * 16, 12 * 16, 32, 32)
			.withOrigin(16, 16)
			.withColor(255, 255, 255, 128 + 128 * sin(mainClock*2));
	}
}

