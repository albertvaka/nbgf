#include "partsys.h"

#include "imgui.h"
#include "rand.h"

void PartSys::Spawn(float dt) {
	time += dt;
	while (time > 0) {
		Particle& p = AddParticle();
		//p.Update(time, *this);
		time -= Random::rollf(min_interval, max_interval);
	}
}

void PartSys::UpdateParticles(float dt) {
	particles.erase(std::remove_if(particles.begin(), particles.end(), [this, dt](Particle& p) { 
		return p.Update(dt, *this);
	}), particles.end());
}

void PartSys::Draw(sf::RenderTarget& rt) const {
	for (const Particle& p : particles) {
		sf::Sprite& spr = sprites[p.sprite];
		spr.setPosition(p.pos);
		spr.setScale(p.scale, p.scale);
		spr.setRotation(p.rotation);
		float alpha = p.alpha;
		if (bounce_alpha  > 0.f && alpha > bounce_alpha) {
			alpha = 2*bounce_alpha - alpha;
		}
		spr.setColor(sf::Color(255, 255, 255, 255*alpha));
		rt.draw(spr);
	}
}

PartSys::Particle& PartSys::AddParticle() {
	particles.push_back({});
	Particle& p = particles.back();
	p.ttl = Random::rollf(min_ttl, max_ttl);
	p.pos = pos;
	p.vel = vec::Rand(min_vel, max_vel);
	p.sprite = Random::roll(sprites.size());
	p.rotation = Random::rollf(min_rotation, max_rotation);
	p.scale = Random::rollf(min_scale, max_scale);
	p.alpha = alpha;
	return p;
}

void PartSys::DrawImGUI(const char* title) {
	// With the interval and ttl ranges here, it's guaranteed we will have ~160 particles or less at a time
	ImGui::Begin(title);
	if (ImGui::SliderFloat("min_interval", &min_interval, 0.05f, 10.f)) {
		if (min_interval > max_interval) {
			max_interval = min_interval;
		}
	}
	if (ImGui::SliderFloat("max_interval", &max_interval, 0.05f, 10.f)) {
		if (min_interval > max_interval) {
			min_interval = max_interval;
		}
	}
	if (ImGui::SliderFloat("min_scale", &min_scale, 0.1f, 8.f)) {
		if (min_scale > max_scale) {
			max_scale = min_scale;
		}
	}
	if (ImGui::SliderFloat("max_scale", &max_scale, 0.1f, 8.f)) {
		if (min_scale > max_scale) {
			min_scale = max_scale;
		}
	}
	ImGui::SliderFloat("scale_vel", &scale_vel, -8.f, 8.f);
	if (ImGui::SliderFloat("min_ttl", &min_ttl, 0.f, 8.f)) {
		if (min_ttl > max_ttl) {
			max_ttl = min_ttl;
		}
	}
	if (ImGui::SliderFloat("maxttl", &max_ttl, 0.f, 8.)) {
		if (min_ttl > max_ttl) {
			min_ttl = max_ttl;
		}
	}
	if (ImGui::SliderFloat2("min_vel", &min_vel.x, -50.f, 50.f)) {
		if (min_vel.x > max_vel.x) {
			max_vel.x = min_vel.x;
		}
		if (min_vel.y > max_vel.y) {
			max_vel.y = min_vel.y;
		}
	}
	if (ImGui::SliderFloat2("max_vel", &max_vel.x, -50.f, 50.f)) {
		if (min_vel.x > max_vel.x) {
			min_vel.x = max_vel.x;
		}
		if (min_vel.y > max_vel.y) {
			min_vel.y = max_vel.y;
		}
	}
	ImGui::SliderFloat("alpha", &alpha, 0.f, 1.f);
	ImGui::SliderFloat("alpha_vel", &alpha_vel, -4.f, 4.f);
	ImGui::SliderFloat("bounce_alpha", &bounce_alpha, -0.1f, 1.f);
	ImGui::SliderFloat("rotation_vel", &rotation_vel, -360.f, 360.f);
	ImGui::SliderFloat2("accel", &acc.x, -50.f, 50.f);
	ImGui::Text("Count: %lu", particles.size());
	ImGui::End();
}