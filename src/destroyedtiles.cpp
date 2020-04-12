#include "destroyedtiles.h"

#include "magic_enum.h"
#include "assets.h"

const float timeToRespawn = 10.f;

DestroyedTiles::DestroyedTiles() {
	for (int i = 0; i < magic_enum::enum_count<Tile::Value>(); ++i) {
		destroyedParticles.AddSprite(Assets::marioTexture, Tile::tileToTextureRect[i]);
	}
	destroyedParticles.acc.y = 50.f;
	destroyedParticles.min_ttl = 0.5f;
	destroyedParticles.max_ttl = 0.5f;
	destroyedParticles.scale_vel = -0.3f;
	destroyedParticles.alpha_vel = -3.f;
	destroyedParticles.min_scale = 0.5;
	destroyedParticles.max_scale = 0.5;
}

const vec displace[4] = {
	{3,-3},
	{3,3},
	{-3,3},
	{-3,-3},
};

const vec vel[4] = {
	{12,-20},
	{12,5},
	{-12,5},
	{-12,-20},
};

void DestroyedTiles::Destroy(int x, int y) {
	TileMap* map = TileMap::instance();

	Tile t = map->getTile(x, y);
	destroyedParticles.pos = map->getTileBounds(x, y).Center();
	for (int i = 0; i < 4; i++) {
		PartSys::Particle& p = destroyedParticles.AddParticle();
		p.pos += displace[i];
		p.vel = vel[i];
		p.sprite = int(t);
	}
	map->setTile(x, y, t == Tile::BREAKABLE_COVERING_ONEWAY ? Tile::ONEWAY_BEHIND_BREAKABLE : Tile::NONE);

	toSpawn.emplace_back(x, y, t, timeToRespawn);
}

void DestroyedTiles::Update(float dt) {
	destroyedParticles.UpdateParticles(dt);
	toSpawn.erase(std::remove_if(toSpawn.begin(), toSpawn.end(), [dt](SpawningTile& t) {
		return t.Update(dt);
	}), toSpawn.end());
}
