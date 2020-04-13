#include "powerups.h"

#include "tilemap.h"
#include "mates.h"
#include "assets.h"

extern sf::Clock mainClock;


void GunUp::Draw(sf::RenderTarget& window) const
{
	float y = 5 * sin(mainClock.getElapsedTime().asSeconds());

	sf::Sprite& spr = Assets::marioSprite;
	spr.setOrigin(0, 0);
	spr.setTextureRect(sf::IntRect(5 * 16, 11 * 16, 16, 16));
	spr.setPosition(pos.x, pos.y - y);
	window.draw(spr);
}



void HealthUp::Draw(sf::RenderTarget& window) const
{
	if (TileMap::instance()->getTile(TileMap::toTiles(pos)).isBreakable()) {
		//Don't draw behind breakables
		return;
	}

	float y = 5 * sin(mainClock.getElapsedTime().asSeconds());

	sf::Sprite& spr = Assets::marioSprite;
	spr.setOrigin(0, 0);
	spr.setTextureRect(sf::IntRect(6 * 16, 11 * 16, 16, 16));
	spr.setPosition(pos.x, pos.y - y);
	window.draw(spr);
}

