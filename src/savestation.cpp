#include "savestation.h"

#include "assets.h"
#include "window_draw.h"
#include "screen.h"
#include "collide.h"
#include "jumpman.h"

#ifdef _DEBUG
#include "text.h"
#endif

extern float mainClock;

std::unordered_map<int, std::vector<SaveStation*>> SaveStation::ByScreen;

SaveStation::SaveStation(int id, vec p) 
	: BoxEntity(p, vec(32, 32)) 
	, id(id)
	, hidden(false)
{ 
	screen = ScreenManager::FindScreenContaining(pos);
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
		}
		return false;
	}

	// Return true if it can be activated
	return Collide(Bounds(), JumpMan::instance()->CollisionBounds());
}

void SaveStation::Draw() const
{
	Window::Draw(Assets::spritesheetTexture, pos)
		.withRect(4 * 16, 12 * 16, 32, 32)
		.withColor(255,255,255, hidden? 128 : 255)
		.withOrigin(16, 16);

	if (!hidden) {
		Window::Draw(Assets::spritesheetTexture, pos)
			.withRect(6 * 16, 12 * 16, 32, 32)
			.withOrigin(16, 16)
			.withColor(255, 255, 255, 128 + 127 * sin(mainClock*2));
	}

#ifdef _DEBUG
	if (Debug::Draw) {
		static Text text(Assets::font_30);
		text.SetString(std::to_string(hiddenBy.size()));
		Window::Draw(text, pos).withScale(0.5f);
	}
#endif _DEBUG
}

