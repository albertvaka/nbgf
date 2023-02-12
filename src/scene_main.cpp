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

float kSimpleEnemyMinDistance = 280;
float kSimpleEnemyMaxDistance = 380;
const float kBossLevelTime = 180.f;
float kLevelTime = 10.f;
float kIntroTime = 0.8f;

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

void MainScene::PlayerDie() {
	Particles::playerExplosion.pos = player.pos;
	Particles::playerExplosion.AddParticles(8);
	Assets::dieSnd.Play();
	player.alive = false;
}

float MainScene::LevelDuration() const {
	if (currentLevel == 10) return kBossLevelTime;
	return kLevelTime;
}

MainScene::MainScene(int level)
	: currentLevel(level)
	, timerText(Assets::font_30, Assets::font_30_outline)
{
	timerText.SetFillColor(0, 0, 0);
	timerText.SetOutlineColor(255, 255, 0);
	Particles::InitParticles();
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
	if (currentLevel == 10) {
		MusicPlayer::Ensure(Assets::bossMusic);
	} else {
		MusicPlayer::Ensure(Assets::gameMusic);
	}
	won = false;
	player.Reset();
	timer = LevelDuration() + kIntroTime;
	switch (currentLevel) {
	case 1:
		new SimpleEnemy(vec(0.33f, 0.3f) * Camera::Size());
		new SimpleEnemy(vec(0.67f, 0.3f) * Camera::Size());
		break;
	case 2:
		new SimpleEnemy(vec(0.33f, 0.2f) * Camera::Size());
		new SimpleEnemy(vec(0.67f, 0.2f) * Camera::Size());
		new SimpleEnemy(vec(0.33f, 0.4f) * Camera::Size(), Angles::Pi / 2);
		new SimpleEnemy(vec(0.67f, 0.4f) * Camera::Size(), Angles::Pi / 2);
		break;
	case 3:
	{
		new StrategyEnemy(AnimLib::ALIEN_BIG, vec(0.2f, 0.2f) * Camera::Size(), multi_shoot_every_sec_strategy, orient_strategy);
		new StrategyEnemy(AnimLib::ALIEN_BIG, vec(0.8f, 0.2f) * Camera::Size(), multi_shoot_every_sec_strategy, orient_strategy);
	}
		break;
	case 4:
	{
		new StrategyEnemy(AnimLib::ALIEN_SIMPLE, vec(0.2f, 0.33f) * Camera::Size(), shoot_player_every_halfsec_strategy, orient_strategy);
		(new StrategyEnemy(AnimLib::ALIEN_DONUT, vec(0.5f, 0.2f) * Camera::Size(), shot_in_circle))->hp = 10;
		new StrategyEnemy(AnimLib::ALIEN_SIMPLE, vec(0.8f, 0.33f) * Camera::Size(), shoot_player_every_halfsec_strategy, orient_strategy);
	}
		break;
	case 5:
	{
		(new StrategyEnemy(AnimLib::ALIEN_TRIANGLE, vec(0.2f, 0.15f) * Camera::Size(), d_shot_every_sec_strategy, orient_strategy))->hp = 8;
		(new StrategyEnemy(AnimLib::ALIEN_TRIANGLE, vec(0.5f, 0.15f) * Camera::Size(), d_shot_every_sec_strategy, orient_strategy))->hp = 8;
		(new StrategyEnemy(AnimLib::ALIEN_TRIANGLE, vec(0.8f, 0.15f) * Camera::Size(), d_shot_every_sec_strategy, orient_strategy))->hp = 8;
	}
	break;
	case 6:
	{
		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.4f, -1.1f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;
		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.6f, -1.1f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;

		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.2f, -0.9f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;
		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.8f, -0.9f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;

		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.4f, -0.7f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;
		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.6f, -0.7f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;

		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.2f, -0.5f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;
		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.8f, -0.5f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;

		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.4f, -0.3f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;
		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.6f, -0.3f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;

		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.2f, -0.1f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;
		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.8f, -0.1f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;
	}
	break;
	case 7:
	{
		(new StrategyEnemy(AnimLib::ALIEN_BIG, vec(0.2f, 0.2f) * Camera::Size(), multi_shoot_every_sec_strategy, orient_strategy))->hp = 3;
		(new StrategyEnemy(AnimLib::ALIEN_KRAKEN, vec(0.5f, 0.3f) * Camera::Size(), octopus_absorb, octopus_wave))->hp = 15;
		(new StrategyEnemy(AnimLib::ALIEN_BIG, vec(0.8f, 0.2f) * Camera::Size(), multi_shoot_every_sec_strategy, orient_strategy))->hp = 3;
	}
	break;
	case 8:
	{
		(new StrategyEnemy(AnimLib::ALIEN_DONUT, vec(0.5f, 0.2f) * Camera::Size(), shot_in_circle))->hp = 10;

		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.25f, -1.0f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;
		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.75f, -1.0f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;

		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.25f, -0.5f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;
		(new StrategyEnemy(AnimLib::ALIEN_CAR, vec(0.75f, -0.5f) * Camera::Size(), fastshoot_player_every_halfsec_strategy, movedown_strategy))->hp = 1;
	}
	break;
	case 9:
	{
		new StrategyEnemy(AnimLib::ALIEN_SIMPLE, vec(0.25f, 0.2f) * Camera::Size(), shoot_player_every_halfsec_strategy, sidetoside_strategy);
		new StrategyEnemy(AnimLib::ALIEN_SIMPLE, vec(0.75f, 0.2f) * Camera::Size(), shoot_player_every_halfsec_strategy, sidetoside_strategy);
		(new StrategyEnemy(AnimLib::ALIEN_TRIANGLE, vec(Camera::Center().x, Camera::Center().y - 20), d_shot_every_sec_strategy, orient_strategy))->hp = 7;
		new StrategyEnemy(AnimLib::ALIEN_SIMPLE, vec(0.25f, 0.6f) * Camera::Size(), shoot_player_every_halfsec_strategy, sidetoside_strategy);
		new StrategyEnemy(AnimLib::ALIEN_SIMPLE, vec(0.75f, 0.6f) * Camera::Size(), shoot_player_every_halfsec_strategy, sidetoside_strategy);
	}
	break;
		break;
	case 10:
	{
		new Boss(vec(0.5f, 0.2f) * Camera::Size(), player);
	}
	break;
	case 11: {
		won = true;
	}
	break;
	}
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
	// Quick access to levels. Starts at SDL_SCANCODE_1.
	for (int i = SDL_SCANCODE_1; i <= SDL_SCANCODE_0; ++i) {
		if (Keyboard::IsKeyJustPressed(static_cast<SDL_Scancode>(i))) {
			currentLevel = 1 + i - SDL_SCANCODE_1;
			SceneManager::RestartScene();
			break;
		}
	}
#endif

	if (SceneManager::newScene != nullptr) return;

	if (won) {
		timerText.SetString("Thanks for playing :D");
	} else {
		timer -= dt;
		if (timer > LevelDuration()) {
			if (timer > LevelDuration() + kIntroTime / 2) {
				timerText.SetString("Ready");
			}
			else {
				timerText.SetString("Set");
			}
			if (timerText.HasChanges()) {
				Assets::readySnd.Play();
			}
			return;
		}
		if (SimpleEnemy::GetAll().empty() && StrategyEnemy::GetAll().empty() && Boss::GetAll().empty()) {
			timerText.SetString("Well done!");
			currentLevel++;
			Fx::FreezeImage::Freeze(1.5f);
			return;
		}

		if (timer <= 0) {
			timerText.SetString("Time out!");
			Fx::FreezeImage::Freeze(2.0f);
			PlayerDie();
			return;
		}

		if (timer > LevelDuration() - kIntroTime) {
			timerText.SetString("Go!");
			if (timerText.HasChanges()) {
				Assets::readySnd.Play();
			}
		}
		else {
			timerText.SetString(Mates::to_string_with_precision(timer, 2));
		}
	}

	for (BackgroundElement* a : BackgroundElement::GetAll()) {
		a->Update(dt);
	}

	player.Update(dt);

	for (Bullet* b : Bullet::GetAll()) {
		b->Update(dt);
	}

	if (won) {
		return;
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
				Fx::FreezeImage::Freeze(0.5f);
				PlayerDie();
			}
		}
		if (a->pos.y > 1.2f * Camera::Size().y) {
			a->alive = false;
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
				Fx::FreezeImage::Freeze(0.5f);
				PlayerDie();
			}
		}
		if (a->pos.y > 1.2f * Camera::Size().y) {
			a->alive = false;
		}
	}

	for (EnemyBullet* b : EnemyBullet::GetAll()) {
		b->Update(dt);
		if (Collide(player.Bounds(), b->Bounds())) {
			if (!player_invincible) {
				Fx::FreezeImage::Freeze(0.5f);
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
	for (Boss* b : Boss::GetAll()) {
		b->Update(dt);
		for (const auto& box : b->actual_colliders) {
			for (Bullet* bullet : Bullet::GetAll()) {
				if (Collide(box.Bounds(), bullet->Bounds())) {
					bullet->alive = false;
					b->Hit();
					break;
				}
			}
			if (Collide(player.Bounds(), box.Bounds())) {
				if (!player_invincible) {
					Fx::FreezeImage::Freeze(0.5f);
					PlayerDie();
				}
			}
		}
	}

	SimpleEnemy::DeleteNotAlive();
	StrategyEnemy::DeleteNotAlive();
	Bullet::DeleteNotAlive();
	EnemyBullet::DeleteNotAlive();
	Boss::DeleteNotAlive();

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
		if (ImGui::SliderInt("level", &currentLevel, 1, 20)) {
			SceneManager::RestartScene();
		};
		ImGui::Checkbox("invincible", &player_invincible);
		
		ImGui::End();
	}

	//Particles::explosion.DrawImGUI();
#endif
	Fx::AfterDraw();

}
