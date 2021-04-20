#pragma once

#include "entity.h"
#include "player.h"
#include "anim_lib.h"
#include "selfregister.h"
#include "particles.h"
#include "enemy_bullet.h"
#include "assets.h"
#include "window.h"
#include "camera.h"


const float scale = 2.0f;
const float turret_bullet_size = 2.0f;
struct Turret {
	vec pos;
	vec offset_pos;
	float mirror = 1.0f;
	float rad = Angles::Tau/4.0f;

	Turret(vec base_pos, vec offset_pos, float mirrored) 
		: offset_pos(offset_pos)
	{
		pos = base_pos + offset_pos;
		mirror = mirrored ? -1.0f : 0.0f;
	}

	void UpdateBasePos(const vec& base_pos) {
		pos = base_pos + offset_pos;
	}

	void Update(float dt, float total_time) {
	}

	vec GetTipPos() const {
		return pos + vec::FromAngleRads(rad) * 16.0f * scale;
	}

	void Draw() const {
			const GPU_Rect& turret_rect = AnimLib::BOSS_TURRET;
			Window::Draw(Assets::spritesTexture, pos)
				.withRect(turret_rect)
				.withOrigin(vec(4.0f, turret_rect.h/2))
				.withRotationRads(rad)
				.withScale(scale);
	}
};

struct BoxCollider : BoxEntity {
	vec offset_pos;
	BoxCollider(vec base_pos, vec offset_pos, vec size) : BoxEntity(size), offset_pos(offset_pos) {
		pos = base_pos + offset_pos;
	}
	void UpdateBasePos(const vec& base_pos) {
		pos = base_pos + offset_pos;
	}
};

struct Boss : SelfRegister<Boss>
{
	const Player& player;
	bool alive = true;
	vec pos;
	vec vel;
	bool can_survive_outbounds = false;
	float total_time = 0;
	std::vector<Turret*> turrets;
	int hp = 200;
	float flashRedTimer = 0.f;
	// todo: add offset to this.
	std::vector<BoxCollider> actual_colliders;

	struct Interval {
		float from;
		float to;
		Interval(float from, float to) : from(from), to(to) {}
		bool IsActive(float total_time) const {
			return total_time >= from && total_time <= to;
		}
	};
	struct TimedAttack {
		Interval interval;
		void (Boss::* shoot_fn)(float);
		TimedAttack(Interval interval, void(Boss::* shoot_fn)(float)) : interval(interval), shoot_fn(shoot_fn) {}
	};
	std::vector<TimedAttack> attacks;

	Boss(const vec& position, const Player& player)
		: player(player)
		, pos(position)
	{
		actual_colliders = {
			BoxCollider(pos, vec(0, -8)*scale, vec(30, 8)*scale),
			BoxCollider(pos, vec(0, -2)*scale, vec(20, 5)*scale),
			BoxCollider(pos, vec(0, -2)*scale, vec(20, 5)*scale),
			BoxCollider(pos, vec(0, 8)*scale, vec(8, 15)*scale),
		};
		bool mirrored = true;
		turrets.push_back(new Turret(pos, vec(-8.0f, -5.0f)*scale, not mirrored));
		turrets.push_back(new Turret(pos, vec(8.0f, -5.0f)*scale, mirrored));

		attacks = {
			TimedAttack({5, 60}, &Boss::ShootDown),
			TimedAttack({10, 20}, &Boss::ShootPlayerWithTurrets),
		};
	}

	~Boss() {
		for (auto* t : turrets) {
			delete t;
		}
	}

	void Hit() {
		hp--;
		flashRedTimer = 0.15f;
		if (hp <= 0) {
			alive = false;
			Particles::explosion.pos = pos;
			Particles::explosion.AddParticles(10);
		} else if ((hp == 50 || hp == 150) && !turrets.empty()) {
			Turret* t = turrets.back();
			Particles::explosion.pos = t->pos;
			Particles::explosion.AddParticles(10);
			turrets.pop_back();
			delete t;
		}
	}

	void ShootDown(float dt) {
		if (ShouldShootWithPeriod(1.0f, total_time, dt)) {
			new EnemyBullet(pos, vec(0, 50));
		}
	}

	void ShootInAxis(float dt) {}

	void ShootPlayerWithTurrets(float dt) {
		vec dir_to_player = (player.pos - pos).Normalized();
		float rad = Angles::Tau/4.0 + Angles::Tau/8.0f * std::sin(total_time);
		for (auto* t : turrets) {
			t->UpdateBasePos(pos);
			t->rad = rad;
			t->Update(dt, total_time);
			if (ShouldShootWithPeriod(0.1f, total_time, dt)) {
				new EnemyBullet(t->GetTipPos(), vec::FromAngleRads(rad) * 40.0f, turret_bullet_size, AnimLib::TURRET_BULLET);
			}
		}
	}

	void Update(float dt)
	{
		total_time += dt;
		flashRedTimer -= dt;

		for (const auto& timed_attack : attacks) {
			if (timed_attack.interval.IsActive(total_time)) {
				(this->*timed_attack.shoot_fn)(dt);
			}
		}
	}

	void Draw() const
	{
		if (flashRedTimer > 0) {
			Assets::tintShader.Activate();
			Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.2f);
		}
		const GPU_Rect& boss_rect = AnimLib::BOSS;
		Window::Draw(Assets::spritesTexture, pos)
			.withRect(boss_rect)
			.withOrigin(vec(boss_rect.w, boss_rect.h) / 2)
			.withScale(2.0f);

		for (auto* t : turrets) {
			t->Draw();
		}
		for (const auto& b : actual_colliders) {
			b.Bounds().DebugDraw();
		}
		Shader::Deactivate();
	}
};