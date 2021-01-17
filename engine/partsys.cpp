#include "partsys.h"

#ifdef _IMGUI
#include "imgui.h"
#endif

#include "rand.h"
#include "window.h"

void PartSys::SpawnWithExternalTimer(float& timer, float dt) {
	timer += dt;
	while (timer > 0) {
		AddParticle();
		//p.Update(time, *this);
		timer -= Rand::rollf(min_interval, max_interval);
	}
}

void PartSys::UpdateParticles(float dt) {
	particles.erase(std::remove_if(particles.begin(), particles.end(), [this, dt](Particle& p) { 
		return p.Update(dt, *this);
	}), particles.end());
}


//Rotation is not supported when USE_VAO is set
//#define USE_VAO

void PartSys::Draw() const {
	for (const Particle& p : particles) {
		float alpha = p.alpha;
		if (bounce_alpha  > 0.f && alpha > bounce_alpha) {
			alpha = 2*bounce_alpha - alpha;
		}

		const GPU_Rect& rect = sprites[p.sprite];
#ifdef USE_VAO
		float w = rect.w * p.scale;
		float h = rect.h * p.scale;
		//vec pos = p.pos - (vec(w, h) * 0.5f);
		RectToTextureCoordinates(texture, rect);
		Window::DrawRaw::BatchColoredTexturedQuad(texture, p.pos.x, p.pos.y, w, h, rect, 1.f,1.f,1.f,p.alpha);
#else
		Window::Draw(texture, p.pos)
			.withColor(255, 255, 255, 255 * alpha)
			.withScale(p.scale)
			.withOrigin(rect.w/2, rect.h/2)
			.withRect(rect)
			.withRotationDegs(p.rotation);
#endif
	}

#ifdef USE_VAO
	Window::DrawRaw::FlushColoredTexturedQuad(texture);
#endif
}

PartSys::Particle& PartSys::AddParticle() {
	particles.push_back({});
	Particle& p = particles.back();
	p.ttl = Rand::rollf(min_ttl, max_ttl);
	p.pos = pos;
	p.vel = Rand::VecInRange(min_vel, max_vel);
	p.sprite = Rand::roll(sprites.size());
	p.rotation = Rand::rollf(min_rotation, max_rotation);
	p.scale = Rand::rollf(min_scale, max_scale);
	p.alpha = alpha;
	return p;
}

void PartSys::DrawImGUI(const char* title) {
#ifdef _IMGUI
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
		if (ImGui::SliderFloat("min_rotation", &min_rotation, 0.f, 360.f)) {
		if (min_rotation > max_rotation) {
			max_rotation = min_rotation;
		}
	}
	if (ImGui::SliderFloat("max_rotation", &max_rotation, 0.f, 360.f)) {
		if (min_rotation > max_rotation) {
			min_rotation = max_rotation;
		}
	}
	ImGui::SliderFloat2("accel", &acc.x, -50.f, 50.f);
	ImGui::Text("Count: %lu", particles.size());
	ImGui::End();
#endif
}