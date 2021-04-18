#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "scene_manager.h"
#include "bullet.h"
#include "enemy_bullet.h"
#include "fx.h"
#include "simple_enemy.h"
#include "strategy_enemy.h"
#include "collide.h"
#include "particles.h"
#include "debug.h"

float kSimpleEnemyMinDistance = 280;
float kSimpleEnemyMaxDistance = 380;
float kLevelTime = 10.f;
float kIntroTime = 0.8f;

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
auto shot_in_circle = [](StrategyEnemy& self, float dt, float total_time) {
	static const int kNumBullets = 30;
	static const float rad_step = Angles::Tau / kNumBullets;
	static const float period = 0.45f;
	if (ShouldShootWithPeriod(period, total_time, dt)) {
		bool angle_parity = static_cast<int>(std::floor(total_time / period)) % 2 == 0;
		float base_rad = angle_parity ? 0 : 0.5f * rad_step;
		angle_parity = !angle_parity;
		for (int i = 0; i < kNumBullets; ++i) {
			new EnemyBullet(self.pos, vec::FromAngleRads(base_rad + i * rad_step) * 60.0f);
		}
	}
};

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
	Fx::FreezeImage::SetUnfreezeCallback([]() {
		SceneManager::RestartScene();
	});
	player.Reset();
	timer = kLevelTime + kIntroTime;
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
		new StrategyEnemy(vec(0.1f, 0.1f) * Camera::Size(), multi_shoot_every_sec_strategy, orient_strategy);
		new StrategyEnemy(vec(0.9f, 0.1f) * Camera::Size(), multi_shoot_every_sec_strategy, orient_strategy);
	}
		break;
	case 4:
	{
		new StrategyEnemy(vec(0.1f, 0.1f) * Camera::Size(), shoot_player_every_halfsec_strategy, orient_strategy);
		new StrategyEnemy(vec(0.9f, 0.1f) * Camera::Size(), shoot_player_every_halfsec_strategy, orient_strategy);
		new StrategyEnemy(vec(0.5f, 0.1f) * Camera::Size(), shot_in_circle);
	}
	break;
	case 5:
	{
		new StrategyEnemy(vec(0.5f, 0.1f)*Camera::Size(), shot_in_circle);
	}
		break;
	case 6:
	{
		new StrategyEnemy(vec(0.1f, 0.1f) * Camera::Size(), d_shot_every_sec_strategy, orient_strategy);
		new StrategyEnemy(vec(0.9f, 0.1f) * Camera::Size(), d_shot_every_sec_strategy, orient_strategy);
		new StrategyEnemy(vec(0.5f, 0.1f) * Camera::Size(), d_shot_every_sec_strategy, orient_strategy);
	}
	break;
	}
}

void MainScene::ExitScene()
{
	Particles::explosion.Clear();
	Bullet::DeleteAll();
	SimpleEnemy::DeleteAll();
	StrategyEnemy::DeleteAll();
	EnemyBullet::DeleteAll();
}

void MainScene::Update(float dt)
{
	if (SceneManager::newScene != nullptr) return;

#ifdef _DEBUG
	const SDL_Scancode restart = SDL_SCANCODE_F5;
	if (Keyboard::IsKeyJustPressed(restart)) {
		ExitScene();
		EnterScene();
		return;
	}
	// Quick access to levels. Starts at SDL_SCANCODE_1.
	for (int i = SDL_SCANCODE_1; i < SDL_SCANCODE_0; ++i) {
		if (Keyboard::IsKeyJustPressed(static_cast<SDL_Scancode>(i))) {
			currentLevel = 1 + i - SDL_SCANCODE_1;
			SceneManager::RestartScene();
			break;
		}
	}
#endif

	timer -= dt;
	if (timer > kLevelTime) {
		if (timer > kLevelTime + kIntroTime / 2) {
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


	if (SimpleEnemy::GetAll().empty() && StrategyEnemy::GetAll().empty()) {
		timerText.SetString("Well done!");
		Fx::FreezeImage::SetAlternativeUpdateFnWhileFrozen([](float dt) {
			Particles::explosion.UpdateParticles(dt);
		});
		currentLevel++;
		Assets::winSnd.Play();
		Fx::FreezeImage::Freeze(2.0f);
		return;
	}

	//lose
	if (timer <= 0) {
		timerText.SetString("Time out!");
		Fx::FreezeImage::Freeze(0.5f);
		Assets::dieSnd.Play();
		return;
	}

	if (timer > kLevelTime - kIntroTime) {
		timerText.SetString("Go!");
		if (timerText.HasChanges()) {
			Assets::goSnd.Play();
		}
	} else {
		timerText.SetString(Mates::to_string_with_precision(timer, 2));
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
	}
	for (StrategyEnemy* a : StrategyEnemy::GetAll()) {
		for (Bullet* b : Bullet::GetAll()) {
			if (Collide(a, b)) {
				b->alive = false;
				a->Hit();
			}
		}
		a->Update(dt);
	}

	for (EnemyBullet* b : EnemyBullet::GetAll()) {
		b->Update(dt);
		if (Collide(player.Bounds(), b->Bounds())) {
			Fx::FreezeImage::Freeze(0.5f);
			Assets::dieSnd.Play();
		}
	}

	SimpleEnemy::DeleteNotAlive();
	StrategyEnemy::DeleteNotAlive();
	Bullet::DeleteNotAlive();
	EnemyBullet::DeleteNotAlive();

	Particles::explosion.UpdateParticles(dt);

}

void MainScene::Draw()
{
	Window::Clear(0, 0, 0);

	Window::Draw(Assets::backgroundTexture, Camera::Center())
		.withOrigin(Assets::backgroundTexture->w/2, Assets::backgroundTexture->h/2);

	/*if (Fx::FreezeImage::IsFrozen()) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
	}*/
	player.Draw();
	//Shader::Deactivate();

	for (const SimpleEnemy* a : SimpleEnemy::GetAll()) {
		a->Draw();
		a->Bounds().DebugDraw(255,0,0);
	}
	for (const auto* a : StrategyEnemy::GetAll()) {
		a->Draw();
		a->Bounds().DebugDraw(255,0,0);
	}

	for (const Bullet* b : Bullet::GetAll()) {
		b->Draw();
		b->Bounds().DebugDraw(255, 0, 0);
	}

	for (const EnemyBullet* b : EnemyBullet::GetAll()) {
		b->Draw();
	}

	Particles::explosion.Draw();

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
		ImGui::End();
	}

	//Particles::explosion.DrawImGUI();
#endif

}
