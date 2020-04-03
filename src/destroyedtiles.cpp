#pragma once

#include "destroyedtiles.h"

DestroyedTiles::DestroyedTiles() {
	for (int i = 0; i < magic_enum::enum_count<Tile>(); ++i) {
		destroyedParticles.AddSprite(Assets::marioTexture, TileMap::tileToTextureRect[i]);
	}
	//destroyedParticles.min_rotation = 0.f;
	//destroyedParticles.rotation_vel = 360.f;
	//destroyedParticles.max_rotation = 360.f;
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
	destroyedParticles.pos = map->tileBounds(x, y).Center();
	for (int i = 0; i < 4; i++) {
		PartSys::Particle& p = destroyedParticles.AddParticle();
		p.pos += displace[i];
		p.vel = vel[i];
		p.sprite = int(t);
	}

	map->setTile(x, y, Tile::NONE);

	destroyed.emplace_back(x, y, 10.f);
}

void DestroyedTiles::Update(float dt) {
	destroyedParticles.UpdateParticles(dt);
	destroyed.erase(std::remove_if(destroyed.begin(), destroyed.end(), [this, dt](Destroyed& p) { 
		p.time -= dt;
		if (p.time < 0.f) {
			TileMap::instance()->setTile(p.x, p.y, Tile::BREAKABLE);
			return true;
		}
		return false;
	}), destroyed.end());
}
