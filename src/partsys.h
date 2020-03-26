#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

#include "vector.h"

struct PartSys {

	struct Particle {
		int sprite;
		vec pos;
		vec vel;
		float ttl;
		float scale;
		float rotation;
		float alpha;
		inline void Update(float dt, const PartSys& system) {
			ttl -= dt;
			vel += system.acc * dt;
			pos += vel * dt;
			scale += system.scale_vel * dt;
			Mates::ClampMin(scale, 0.0001f);
			rotation += system.rotation_vel * dt;
			alpha += system.alpha_vel * dt;
			Mates::Clamp(alpha, 0.f, 1.f);
		}
	};

	vec pos = vec();

	vec max_vel = vec();
	vec min_vel = vec();

	vec acc = vec();

	float min_ttl = 1.f;
	float max_ttl = 1.f;

	float min_interval = 0.2f;
	float max_interval = 0.2f;

	float min_scale = 1.f;
	float max_scale = 1.f;

	float scale_vel = 0.f;

	float min_rotation = 0.f;
	float max_rotation = 0.f;

	float rotation_vel = 0.f;

	float alpha = 1.f;
	float alpha_vel = 0.f;

	float time = 0.f;

	void AddSprite(const sf::Texture& texture, const sf::IntRect& rect) {
		sprites.emplace_back(texture, rect);
		sprites.back().setOrigin(rect.width/2,rect.height/2);
	}

	void Spawn(float dt);
	void UpdateParticles(float dt); //Doesn't create new particles, use Spawn() 
	void Draw(sf::RenderTarget& rt);

	Particle& AddParticle();
	inline void AddParticles(int n) {
		while (n-- > 0) {
			AddParticle();
		}
	}

	void DrawImGUI(const char* title = "Particles");
	void Clear() { particles.clear(); }

	void FlipX() {
		float aux = max_vel.x;
		max_vel.x = -min_vel.x;
		min_vel.x = -aux;
		acc.x = -acc.x;
	}

private:
	std::vector<Particle> particles;
	std::vector<sf::Sprite> sprites;
};

