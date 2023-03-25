#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "scene_manager.h"
#include "boss.h"
#include "bullet.h"
#include "enemy_bullet.h"
#include "fx.h"
#include "simple_enemy.h"
#include "strategy_enemy.h"
#include "collide.h"
#include "particles.h"
#include "backgroundelement.h"
#include "debug.h"

#include <nats/nats.h>

float kSimpleEnemyMinDistance = 280;
float kSimpleEnemyMaxDistance = 380;
float kLevelTime = 10.f;

const static char mynum = 13;

natsConnection      *conn  = NULL;

auto octopus_wave = [](StrategyEnemy& self, float dt) {
	self.pos.y = self.initialPos.y - sin(self.total_time*2) * 12;
};
auto sidetoside_strategy = [](StrategyEnemy& self, float dt) {
	self.pos.x = Mates::map(cos(self.total_time+Angles::Tau/2), -1, 1.4f, self.initialPos.x, Camera::Center().x);
	self.pos.y = Mates::map(cos(self.total_time+Angles::Tau/2), -1, 1.6f, self.initialPos.y, Camera::Center().y- 20);
	vec dir = Player::instance()->pos - self.pos;
	self.rot_rads = -Angles::Pi / 2.0f + std::atan2(dir.y, dir.x);
};
auto movedown_strategy = [](StrategyEnemy& self, float dt) {
	self.pos.y += 100 * dt;
	self.pos.x = self.initialPos.x + sin(self.total_time*2) * 20;
};
auto fastshoot_player_every_halfsec_strategy = [](StrategyEnemy& self, float dt, float total_time) {
	if (ShouldShootWithPeriod(0.4f, total_time, dt)) { new EnemyBullet(self.pos, (Player::instance()->pos - self.pos).Normalized() * 160.0f); }
};
auto orient_strategy = [](StrategyEnemy& self, float dt) {
	vec dir = Player::instance()->pos - self.pos;
	// Add 90 deg, since it's facing south (90 deg) by default.
	self.rot_rads = -Angles::Pi / 2.0f + std::atan2(dir.y, dir.x);
};
auto shoot_player_every_halfsec_strategy = [](StrategyEnemy& self, float dt, float total_time) {
	if (ShouldShootWithPeriod(0.5f, total_time, dt)) { new EnemyBullet(self.pos, (Player::instance()->pos - self.pos).Normalized() * 80.0f); }
};
auto multi_shoot_every_sec_strategy = [](StrategyEnemy& self, float dt, float total_time) {
	if (ShouldShootWithPeriod(1.0f, total_time - 0.2f, dt)) { new EnemyBullet(self.pos, (Player::instance()->pos - self.pos).Normalized().RotatedAroundOriginDegs(20) * 80.0f); }
	if (ShouldShootWithPeriod(1.0f, total_time - 0.1f, dt)) { new EnemyBullet(self.pos, (Player::instance()->pos - self.pos).Normalized().RotatedAroundOriginDegs(10) * 80.0f); }
	if (ShouldShootWithPeriod(1.0f, total_time,        dt)) { new EnemyBullet(self.pos, (Player::instance()->pos - self.pos).Normalized() * 80.0f); }
	if (ShouldShootWithPeriod(1.0f, total_time + 0.1f, dt)) { new EnemyBullet(self.pos, (Player::instance()->pos - self.pos).Normalized().RotatedAroundOriginDegs(-10) * 80.0f); }
	if (ShouldShootWithPeriod(1.0f, total_time + 0.2f, dt)) { new EnemyBullet(self.pos, (Player::instance()->pos - self.pos).Normalized().RotatedAroundOriginDegs(-20) * 80.0f); }
};
auto d_shot_every_sec_strategy = [](StrategyEnemy& self, float dt, float total_time) {
	if (ShouldShootWithPeriod(.7f, total_time, dt)) {
		vec playerDir = (Player::instance()->pos - self.pos).Normalized();
		new EnemyBullet(self.pos + (playerDir * 25).RotatedAroundOriginDegs(60), playerDir * 80.0f);
		new EnemyBullet(self.pos + (playerDir * 25).RotatedAroundOriginDegs(50), playerDir * 80.0f);
		new EnemyBullet(self.pos + (playerDir * 25).RotatedAroundOriginDegs(40), playerDir * 80.0f);
		new EnemyBullet(self.pos + (playerDir * 25).RotatedAroundOriginDegs(30), playerDir * 80.0f);
		new EnemyBullet(self.pos + (playerDir * 25).RotatedAroundOriginDegs(20), playerDir * 80.0f);
		new EnemyBullet(self.pos + (playerDir * 25).RotatedAroundOriginDegs(10), playerDir * 80.0f);
		new EnemyBullet(self.pos + (playerDir * 25), playerDir * 80.0f);
		new EnemyBullet(self.pos + (playerDir * 25).RotatedAroundOriginDegs(-10), playerDir * 80.0f);
		new EnemyBullet(self.pos + (playerDir * 25).RotatedAroundOriginDegs(-20), playerDir * 80.0f);
		new EnemyBullet(self.pos + (playerDir * 25).RotatedAroundOriginDegs(-30), playerDir * 80.0f);
		new EnemyBullet(self.pos + (playerDir * 25).RotatedAroundOriginDegs(-40), playerDir * 80.0f);
		new EnemyBullet(self.pos + (playerDir * 25).RotatedAroundOriginDegs(-50), playerDir * 80.0f);
		new EnemyBullet(self.pos + (playerDir * 25).RotatedAroundOriginDegs(-60), playerDir * 80.0f);
	}
};

auto octopus_absorb = [](StrategyEnemy& self, float dt, float total_time) {
	static const int kNumBullets = 11;
	static const float period = 0.5f;
	static const float rad_step = Angles::Pi / kNumBullets;
	// TODO: Draw under the ships.
	if (ShouldShootWithPeriod(period, total_time, dt)) {
		for (int i = 2; i < kNumBullets-2; ++i) {
			bool angle_parity = static_cast<int>(std::floor(total_time / period)) % 2 == 0;
			float base_rad = angle_parity ? 0 : 0.5f * rad_step;
			auto dir = vec::FromAngleRads(base_rad + i*rad_step);
			(new EnemyBullet(self.pos + dir * Window::GAME_WIDTH*0.75f, -dir * 55))->can_survive_outbounds=true;
		}
	}
};

auto shot_in_circle = [](StrategyEnemy& self, float dt, float total_time) {
	static const int kNumBullets = 28;
	static const float rad_step = Angles::Tau / kNumBullets;
	static const float period = 0.6f;
	if (ShouldShootWithPeriod(period, total_time, dt)) {
		bool angle_parity = static_cast<int>(std::floor(total_time / period)) % 2 == 0;
		float base_rad = angle_parity ? 0 : 0.5f * rad_step;
		angle_parity = !angle_parity;
		for (int i = 0; i < kNumBullets; ++i) {
			new EnemyBullet(self.pos, vec::FromAngleRads(base_rad + i * rad_step) * 60.0f);
		}
	}
};

#include "window_draw.h"

void spawn(char c) {
	vec pos = Rand::VecInRange(Camera::Bounds());
	switch (c) {
	case 0:
	case 1:
		new SimpleEnemy(c, pos);
		break;
	case 2:
	case 3:
	{
		new StrategyEnemy(c, AnimLib::ALIEN_BIG, pos, multi_shoot_every_sec_strategy, orient_strategy);
	}
	break;
	case 4:
	case 5:
	{
		new StrategyEnemy(c, AnimLib::ALIEN_SIMPLE, pos, shoot_player_every_halfsec_strategy, orient_strategy);
	}
		break;
	case 6:
	case 7:
	{
		(new StrategyEnemy(c, AnimLib::ALIEN_TRIANGLE, pos, d_shot_every_sec_strategy, orient_strategy))->hp = 1;
	}
	break;
	case 8:
	case 9:
	case 10:
	{
		new StrategyEnemy(c, AnimLib::ALIEN_SIMPLE, pos, shoot_player_every_halfsec_strategy, sidetoside_strategy);
	}
	break;
		break;
	break;
	default:
	{
		Debug::out << "Not an enemy: " << (int)c;
	}
	break;
	}

}

static void onMsg(natsConnection *nc, natsSubscription *sub, natsMsg *msg, void *closure)
{
	vec pos = vec(Rand::rollf(Window::GAME_WIDTH, Window::GAME_HEIGHT));
	int length = natsMsg_GetDataLength(msg);
	const char * data = natsMsg_GetData(msg);
	Debug::out << "len=" << length << " num=" << std::to_string(data[0]) << " msg=" << std::to_string(data[1]);
	if (data[0] == mynum) {
		Debug::out << "Ignoring";
		return;
	}
	spawn(data[1]);

    natsMsg_Destroy(msg);
}

void senderino(char n) {
	char tosend[] = {mynum, n};
	natsStatus s = natsConnection_Publish(conn, "fml", &tosend, 2);
	if (s != NATS_OK) {
		nats_PrintLastErrorStack(stderr);
	}
}

void MainScene::PlayerDie() {
	Fx::FreezeImage::Freeze(0.5f);
	Particles::playerExplosion.pos = player.pos;
	Particles::playerExplosion.AddParticles(8);
	Assets::dieSnd.Play();
	player.alive = false;
}


MainScene::MainScene()
	: timerText(Assets::font_30, Assets::font_30_outline)
{
	timerText.SetFillColor(0, 0, 0);
	timerText.SetOutlineColor(255, 255, 0);
	Particles::InitParticles();

    natsStatus s = natsConnection_ConnectTo(&conn, "nats://demo.nats.io:4222");
	if (s != NATS_OK) {
		nats_PrintLastErrorStack(stderr);
	}
    natsSubscription    *sub   = NULL;
    s = natsConnection_Subscribe(&sub, conn, "fml", onMsg, NULL);
	if (s != NATS_OK) {
		nats_PrintLastErrorStack(stderr);
	}
}

void MainScene::EnterScene() 
{
	Fx::BeforeEnterScene();
	Fx::FreezeImage::SetUnfreezeCallback([]() {
		SceneManager::RestartScene();
	});
	Fx::FreezeImage::SetAlternativeUpdateFnWhileFrozen([](float dt) {
		Particles::explosion.UpdateParticles(dt);
		Particles::playerExplosion.UpdateParticles(dt);
	});
	for (int i = 0; i < BackgroundElement::kNumBackgroundElements; i++) {
		new BackgroundElement();
	}
	MusicPlayer::Ensure(Assets::gameMusic);
	player.Reset();
	spawn(Rand::roll(10));
	spawn(Rand::roll(10));
	spawn(Rand::roll(10));
	spawn(Rand::roll(10));
}

void MainScene::ExitScene()
{
	Particles::explosion.Clear();
	Particles::playerExplosion.Clear();
	Bullet::DeleteAll();
	SimpleEnemy::DeleteAll();
	StrategyEnemy::DeleteAll();
	EnemyBullet::DeleteAll();
	BackgroundElement::DeleteAll();
	Boss::DeleteAll();
}

void MainScene::Update(float dt)
{
	Fx::Update(dt);
	bool frozen = Fx::FreezeImage::IsFrozen();
	if (frozen) {
		return;
	}
	MusicPlayer::ToggleMusicWithM();
#ifdef _DEBUG
	const SDL_Scancode restart = SDL_SCANCODE_F5;
	if (Keyboard::IsKeyJustPressed(restart)) {
		ExitScene();
		EnterScene();
		return;
	}
#endif

	for (BackgroundElement* a : BackgroundElement::GetAll()) {
		a->Update(dt);
	}

	player.Update(dt);

	for (Bullet* b : Bullet::GetAll()) {
		b->Update(dt);
	}


	for (SimpleEnemy* a : SimpleEnemy::GetAll()) {
		for (Bullet* b : Bullet::GetAll()) {
			if (Collide(a, b)) {
				b->alive = false;
				a->Hit();
			}
		}
		a->Update(dt);
		if (Collide(player.Bounds(), a->Bounds())) {
			if (!player_invincible) {
				PlayerDie();
			}
		}
	}
	for (StrategyEnemy* a : StrategyEnemy::GetAll()) {
		for (Bullet* b : Bullet::GetAll()) {
			if (Collide(a, b)) {
				b->alive = false;
				a->Hit();
			}
		}
		a->Update(dt);
		if (Collide(player.Bounds(), a->Bounds())) {
			if (!player_invincible) {
				PlayerDie();
			}
		}
	}

	for (EnemyBullet* b : EnemyBullet::GetAll()) {
		b->Update(dt);
		if (Collide(player.Bounds(), b->Bounds())) {
			if (!player_invincible) {
				PlayerDie();
			}
		}
		// Absorved bullets.
		if (b->can_survive_outbounds) {
			for (const auto *enemy : StrategyEnemy::GetAll()) {
				if (Collide(b->Bounds(), enemy->Bounds())) {
					b->alive = false;
				}
			}
		}
	}

	for (SimpleEnemy* a : SimpleEnemy::GetAll()) {
		if (!a->alive) senderino(a->c);
	}
	for (StrategyEnemy* a : StrategyEnemy::GetAll()) {
		if (!a->alive) senderino(a->c);
	}
	SimpleEnemy::DeleteNotAlive();
	StrategyEnemy::DeleteNotAlive();
	Bullet::DeleteNotAlive();
	EnemyBullet::DeleteNotAlive();

	Particles::explosion.UpdateParticles(dt);
	Particles::playerExplosion.UpdateParticles(dt);

}

void MainScene::Draw()
{
	Window::Clear(15, 15, 15);

	for (const BackgroundElement* a : BackgroundElement::GetAll()) {
		a->Draw();
	}

	player.Draw();

	for (const SimpleEnemy* a : SimpleEnemy::GetAll()) {
		a->Draw();
		a->Bounds().DebugDraw(255,0,0);
	}
	for (const auto* a : StrategyEnemy::GetAll()) {
		a->Draw();
		a->Bounds().DebugDraw(255,0,0);
	}
	for (const auto* b : Boss::GetAll()) {
		b->Draw();
	}

	for (const Bullet* b : Bullet::GetAll()) {
		b->Draw();
		b->Bounds().DebugDraw(255, 0, 0);
	}

	for (const EnemyBullet* b : EnemyBullet::GetAll()) {
		b->Draw();
	}

	Particles::explosion.Draw();
	Particles::playerExplosion.Draw();

	Window::Draw(timerText, vec(Camera::Center().x, 12))
		.withOrigin(timerText.Size()/2)
		.withScale(0.35f);

	if (Debug::Draw) {
		Window::DrawPrimitive::Circle(Camera::Center(), kSimpleEnemyMinDistance, 1, 255, 255, 255);
		Window::DrawPrimitive::Circle(Camera::Center(), kSimpleEnemyMaxDistance, 1, 255, 255, 255);
	}

#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::Text("Enemies: %d", (SimpleEnemy::GetAll().size()+StrategyEnemy::GetAll().size()));
		ImGui::Text("Bullets: %d", EnemyBullet::GetAll().size());
		ImGui::Checkbox("invincible", &player_invincible);

		ImGui::End();
	}

	//Particles::explosion.DrawImGUI();
#endif
	Fx::AfterDraw();

}
