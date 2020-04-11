#include "spawningtile.h"

#include "jumpman.h"
#include "assets.h"
#include "collider.h"
#include "bat.h"
#include "debug.h"

const float respawnAnimTime = 0.2f;

bool SpawningTile::CanSpawn() const {
	if (Collide(JumpMan::instance()->maxBounds(), this->bounds())) return false;
	for (Bat* b : Bat::getAll()) {
		if (Collide(b->bounds(), this->bounds())) return false;
	}
	return true;
}

void SpawningTile::Draw(sf::RenderTarget& window) const {
	if (Debug::Draw) {
		JumpMan::instance()->maxBounds().Draw(window, sf::Color::Blue);
		this->bounds().Draw(window, sf::Color::Black);
	}
	if (spawning) {
		sf::Sprite& spr = Assets::marioSprite;
		spr.setTextureRect(tile.textureRect());
		float scale = 1.f - time / respawnAnimTime;
		spr.setScale(scale, scale);
		spr.setOrigin(Tile::size / 2, Tile::size / 2);
		spr.setPosition(pos.x, pos.y);
		window.draw(spr);
	}
}

bool SpawningTile::Update(float dt) { //Returns true if it should be destroyed
	time -= dt;
	if (time < 0.f) {
		if (spawning) {
			TileMap::instance()->setTile(TileMap::toTiles(pos), tile);
			return true;
		}
		else if (CanSpawn()) {
			spawning = true;
			time = respawnAnimTime;
			TileMap::instance()->setTile(TileMap::toTiles(pos), Tile::SOLID_TRANSPARENT);
		}
	}
	return false;
}