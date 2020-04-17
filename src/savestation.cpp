#include "savestation.h"

#include "tilemap.h"
#include "mates.h"
#include "assets.h"
#include "input.h"

extern sf::Clock mainClock;

SaveStation::SaveStation(const vec& p) : BoxEntity(p, vec(32, 32)) { }

void SaveStation::Update(float dt) {
	bool prevInScene = inScene;
	inScene = Camera::GetCameraBounds().intersects(bounds());
	if (inScene && !prevInScene) {
		enabled = true;
	}
}

void SaveStation::Activate() {
	if (enabled) {
		enabled = false;
	}
}

void SaveStation::Draw(sf::RenderTarget& window) const
{
	sf::Sprite& spr = Assets::marioSprite;

	spr.setOrigin(16, 16);
	spr.setTextureRect(sf::IntRect(4 * 16, 12 * 16, 32, 32));
	spr.setPosition(pos.x, pos.y);
	window.draw(spr);

	if (enabled) {
		spr.setTextureRect(sf::IntRect(6 * 16, 12 * 16, 32, 32));
		spr.setColor(sf::Color(255, 255, 255, 128 + 128 * sin(mainClock.getElapsedTime().asSeconds()*2)));
		spr.setPosition(pos.x, pos.y);
		window.draw(spr);
		spr.setColor(sf::Color(255, 255, 255, 255));
	}
}

