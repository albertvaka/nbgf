#include "destroyedtiles.h"

#include <algorithm>
#include "magic_enum.h"
#include "gaemtilemap.h"
#include "assets.h"

constexpr float kTimeToRespawn = 10.f;
constexpr float kTimeToRespawnHard = 4.5f;

DestroyedTiles::DestroyedTiles()
	: destroyedParticles(Assets::spritesheetTexture)
{
	for (size_t i = 0; i < magic_enum::enum_count<Tile::Value>(); ++i) {
		destroyedParticles.AddSprite(Tile::TileToTextureRect[i]);
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

void DestroyedTiles::Destroy(int x, int y, bool respawn)
{

	GaemTileMap* map = GaemTileMap::instance();
	Tile t = map->GetTile(x, y);

	if (t == Tile::NONE) {
		return;
	}

	map->SetTile(x, y, t.GetTileBehind());

	//if (animated) {
		destroyedParticles.pos = Tile::Bounds(x, y).Center();
		for (int i = 0; i < 4; i++) {
			PartSys::Particle& p = destroyedParticles.AddParticle();
			p.pos += displace[i];
			p.vel = vel[i];
			p.sprite = int(t);
		}
	//}

	if (respawn) {
		toSpawn.emplace_back(x, y, t, t.isBreakable(Tile::BreakResistance::HARD)? kTimeToRespawnHard : kTimeToRespawn);
	} else {
		permanentlyDestroyed.emplace_back(x, y);
	}

	if (t.IsChainBreakable()) {
		veci neigbours[4] = { veci(x + 1, y), veci(x - 1, y), veci(x, y + 1), veci(x, y - 1) };
		for (auto neighbour : neigbours) {
			if (map->GetTile(neighbour) == t) {
				Destroy(neighbour.x, neighbour.y, respawn);
			}
		}
	}
}

void DestroyedTiles::Update(float dt)
{
	destroyedParticles.UpdateParticles(dt);
	toSpawn.erase(std::remove_if(toSpawn.begin(), toSpawn.end(), [dt](SpawningTile& t) {
		return t.Update(dt);
	}), toSpawn.end());
}

void DestroyedTiles::SaveGame(SaveState& save) const
{
	auto s = save.StreamPut("destroyed_tiles");
	for (veci v : permanentlyDestroyed) {
		s << v.x << v.y;
	}
}

void DestroyedTiles::LoadGame(const SaveState& save)
{
	auto s = save.StreamGet("destroyed_tiles");
	int x, y;
	GaemTileMap* map = GaemTileMap::instance();
	while (s >> x >> y) {
		permanentlyDestroyed.emplace_back(x, y);
		Tile t = map->GetTile(x, y);
		map->SetTile(x, y, t.GetTileBehind());
	}
}
