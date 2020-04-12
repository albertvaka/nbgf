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
		inline bool Update(float dt, const PartSys& system) {
			ttl -= dt;
			if (ttl < 0) return true;
			vel += system.acc * dt;
			pos += vel * dt;
			scale += system.scale_vel * dt;
			if (scale < 0.f) {
				if (system.scale_vel < 0.f) {
					return true;
				}
				else {
					scale = 0.0001f;
				}
			}
			rotation += system.rotation_vel * dt;
			alpha += system.alpha_vel * dt;
			if (alpha < 0.f) {
				if (system.alpha_vel < 0.f) {
					return true;
				} else {
					alpha = 0.f;
				}
			}
			else if (alpha > 1.f) {
				alpha = 1.f;
			}
			return false;
		}
	};

	vec pos = vec();
	float spawn_radius = 0.f; // TODO: Implement

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

	sf::Sprite& AddSprite(const sf::Texture& texture, const sf::IntRect& rect) {
		sprites.emplace_back(texture, rect);
		sf::Sprite& sprite = sprites.back();
		sprite.setOrigin(rect.width/2.f,rect.height/2.f);
		return sprite;
	}

	void Spawn(float dt);
	void UpdateParticles(float dt); //Doesn't create new particles, use Spawn() 
	void Draw(sf::RenderTarget& rt) const;

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
	//TODO: Turn into static arrays
	std::vector<Particle> particles;
	mutable std::vector<sf::Sprite> sprites;
};

