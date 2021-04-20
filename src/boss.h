#pragma once

#include "entity.h"
#include "player.h"
#include "anim_lib.h"
#include "selfregister.h"
#include "particles.h"
#include "enemy_bullet.h"
#include "assets.h"
#include "window.h"
#include "rand.h"
#include "camera.h"

const int kHealth = 200;
const int kHealthLosesFirstTurret = 150;
const int kHealthLosesSecondTurret = 50;


const float scale = 2.0f;
const float turret_bullet_size = 2.0f;
struct Turret {
	vec pos;
	vec offset_pos;
	float rad = Angles::Tau/4.0f;

	Turret(vec base_pos, vec offset_pos) 
		: offset_pos(offset_pos)
	{
		pos = base_pos + offset_pos;
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

struct Boss : Entity, SelfRegister<Boss>
{
	const Player& player;
	vec initialPos;
	bool can_survive_outbounds = false;
	float total_time = 0.f;
	std::vector<Turret*> turrets;
	int hp = kHealth;
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
		: Entity(position)
		, player(player)
		, initialPos(position)
	{
		actual_colliders = {
			BoxCollider(pos, vec(0, -8)*scale, vec(30, 8)*scale),
			BoxCollider(pos, vec(0, -2)*scale, vec(20, 5)*scale),
			BoxCollider(pos, vec(0, -2)*scale, vec(20, 5)*scale),
			BoxCollider(pos, vec(0, 8)*scale, vec(8, 15)*scale),
		};
		turrets.push_back(new Turret(pos, vec(-8.0f, -5.0f)*scale));
		turrets.push_back(new Turret(pos, vec(8.0f, -5.0f)*scale));

		attacks = {
			TimedAttack({0, 200.f}, &Boss::ShootDown),
			TimedAttack({10, 20}, &Boss::ShootPlayerWithTurrets),
			TimedAttack({30, 40}, &Boss::ShootPlayerWithTurrets),
			TimedAttack({50, 60}, &Boss::ShootPlayerWithTurrets),
			TimedAttack({70, 80}, &Boss::ShootPlayerWithTurrets),
			TimedAttack({90, 100}, &Boss::ShootPlayerWithTurrets),
			TimedAttack({110, 120}, &Boss::ShootPlayerWithTurrets),
			TimedAttack({130, 140}, &Boss::ShootPlayerWithTurrets),
			TimedAttack({150, 160}, &Boss::ShootPlayerWithTurrets),
			TimedAttack({170, 180}, &Boss::ShootPlayerWithTurrets),
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
		} else if ((hp == kHealthLosesFirstTurret || hp == kHealthLosesSecondTurret) && !turrets.empty()) {
			Turret* t = turrets.back();
			Particles::explosion.pos = t->pos;
			Particles::explosion.AddParticles(10);
			turrets.pop_back();
			delete t;
		}
	}

	int shots = 0;
	void ShootDown(float dt) {
		float period = 1.0f;
		if (hp < kHealthLosesFirstTurret) {
			period /= 2;
			if (hp < kHealthLosesSecondTurret) {
				period /= 2;
			}
		}
		if (ShouldShootWithPeriod(period, total_time, dt)) {
			//if (shots == 4) {
				//shots = 0;
				vec dir = vec(0,1);
				new EnemyBullet(pos + (dir * 25).RotatedAroundOriginDegs(60), dir * 80.0f);
				new EnemyBullet(pos + (dir * 25).RotatedAroundOriginDegs(50), dir * 80.0f);
				new EnemyBullet(pos + (dir * 25).RotatedAroundOriginDegs(40), dir * 80.0f);
				new EnemyBullet(pos + (dir * 25).RotatedAroundOriginDegs(30), dir * 80.0f);
				new EnemyBullet(pos + (dir * 25).RotatedAroundOriginDegs(20), dir * 80.0f);
				new EnemyBullet(pos + (dir * 25).RotatedAroundOriginDegs(10), dir * 80.0f);
				new EnemyBullet(pos + (dir * 25), dir * 80.0f);
				new EnemyBullet(pos + (dir * 25).RotatedAroundOriginDegs(-10), dir * 80.0f);
				new EnemyBullet(pos + (dir * 25).RotatedAroundOriginDegs(-20), dir * 80.0f);
				new EnemyBullet(pos + (dir * 25).RotatedAroundOriginDegs(-30), dir * 80.0f);
				new EnemyBullet(pos + (dir * 25).RotatedAroundOriginDegs(-40), dir * 80.0f);
				new EnemyBullet(pos + (dir * 25).RotatedAroundOriginDegs(-50), dir * 80.0f);
				new EnemyBullet(pos + (dir * 25).RotatedAroundOriginDegs(-60), dir * 80.0f);
			//} else {
			//	shots++;
			//	new EnemyBullet(pos - vec(5,0), vec(0, 80));
			//	new EnemyBullet(pos + vec(5,0), vec(0, 80));
			//}
		}
		if (ShouldShootWithPeriod(period, total_time+0.5f, dt)) {
			new EnemyBullet(pos, (player.pos-pos).Normalized()*120);
		}
	}

	void ShootInAxis(float dt) {}

	void ShootPlayerWithTurrets(float dt) {
		float rad = Angles::Tau/4.0 + Angles::Tau/8.0f * sin(Mates::map((int(total_time*1000)%10000)/1000.f, 0.f, 10.f, 0.f, 2*Angles::Tau));
		for (auto* t : turrets) {
			t->rad = rad;
			t->Update(dt, total_time);
			if (ShouldShootWithPeriod(0.1f, total_time, dt)) {
				new EnemyBullet(t->GetTipPos(), vec::FromAngleRads(rad) * 40.0f, turret_bullet_size, AnimLib::TURRET_BULLET);
			}
		}
	}

	void Update(float dt)
	{
		pos.x = initialPos.x + sin(total_time*0.666f)*(Camera::Size().x/2.5f);
		pos.y = initialPos.y + sin(total_time*2)*20;

		for (auto* t : turrets) {
			t->UpdateBasePos(pos);
		}

		for (auto& t : actual_colliders) {
			t.UpdateBasePos(pos);
		}

		for (const auto& timed_attack : attacks) {
			if (timed_attack.interval.IsActive(total_time)) {
				(this->*timed_attack.shoot_fn)(dt);
			}
		}

		total_time += dt;
		flashRedTimer -= dt;
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
