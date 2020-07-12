#include "spawningtile.h"

#include "jumpman.h"
#include "assets.h"
#include "collide.h"
#include "bat.h"
#include "window.h"
#include "debug.h"

const float respawnAnimTime = 0.2f;

bool SpawningTile::CanSpawn() const {
	if (Collide(JumpMan::instance()->maxBounds(), this->bounds())) return false;
	for (Bat* b : Bat::GetAll()) {
		if (Collide(b->bounds(), this->bounds())) return false;
	}
	return true;
}

void SpawningTile::Draw() const {
	if (Debug::Draw) {
		JumpMan::instance()->maxBounds().Draw(0,0,255);
		this->bounds().Draw(0, 0, 0);
	}
	if (spawning) {
		Window::Draw(Assets::marioTexture, pos)
			.withScale(1.f - time / respawnAnimTime)
			.withOrigin(Tile::size / 2, Tile::size / 2)
			.withRect(tile.textureRect());
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