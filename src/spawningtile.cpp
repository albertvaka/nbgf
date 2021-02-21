#include "spawningtile.h"

#include "jumpman.h"
#include "assets.h"
#include "collide.h"
#include "bat.h"
#include "gaemtilemap.h"
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
		Window::Draw(Assets::spritesheetTexture, pos)
			.withScale(1.f - time / respawnAnimTime)
			.withOrigin(Tile::Size / 2, Tile::Size / 2)
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
			GaemTileMap::instance()->SetTile(Tile::ToTiles(pos), tile);
			return true;
		}
		else if (CanSpawn()) {
			spawning = true;
			time = respawnAnimTime;
			GaemTileMap::instance()->SetTile(Tile::ToTiles(pos), Tile::SOLID_TRANSPARENT);
		}
	}
	return false;
}