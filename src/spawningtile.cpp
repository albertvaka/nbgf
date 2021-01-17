#include "spawningtile.h"

#include "jumpman.h"
#include "assets.h"
#include "collide.h"
#include "bat.h"
#include "window.h"
#include "debug.h"

const float respawnAnimTime = 0.2f;

bool SpawningTile::CanSpawn() const {
	if (Collide(JumpMan::instance()->MaxBounds(), this->Bounds())) return false;
	for (Bat* b : Bat::GetAll()) {
		if (Collide(b->Bounds(), this->Bounds())) return false;
	}
	return true;
}

void SpawningTile::Draw() const {
	if (spawning) {
		Window::Draw(Assets::marioTexture, pos)
			.withScale(1.f - time / respawnAnimTime)
			.withOrigin(Tile::size / 2, Tile::size / 2)
			.withRect(tile.textureRect());
	}

	// Debug-only
	JumpMan::instance()->MaxBounds().DebugDraw(0, 0, 255);
	this->Bounds().DebugDraw(0, 0, 0);
}

bool SpawningTile::Update(float dt) { //Returns true if it should be destroyed
	time -= dt;
	if (time < 0.f) {
		if (spawning) {
			TileMap::instance()->SetTile(TileMap::ToTiles(pos), tile);
			return true;
		}
		else if (CanSpawn()) {
			spawning = true;
			time = respawnAnimTime;
			TileMap::instance()->SetTile(TileMap::ToTiles(pos), Tile::SOLID_TRANSPARENT);
		}
	}
	return false;
}