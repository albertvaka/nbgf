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
#include "debug.h"

float kSimpleEnemyMinDistance = 280;
float kSimpleEnemyMaxDistance = 380;
float kLevelTime = 10.f;
float kIntroTime = 0.8f;

MainScene::MainScene(int level)
	: player(vec::Zero)
	, currentLevel(level)
	, alienPartSys(Assets::invadersTexture)
	, timerText(Assets::font_30, Assets::font_30_outline)
{
	timerText.SetFillColor(0, 0, 0);
	timerText.SetOutlineColor(255, 255, 0);
	alienPartSys.AddSprite(AnimLib::ALIEN_1[0].rect);
	alienPartSys.AddSprite(AnimLib::ALIEN_2[0].rect);
	alienPartSys.min_scale = 0.15f;
	alienPartSys.max_scale = 0.4f;
	alienPartSys.max_vel = vec(50,50);
	alienPartSys.min_vel = vec(-50,-50);
	alienPartSys.min_rotation = 0;
	alienPartSys.max_rotation = 360;
	alienPartSys.rotation_vel = 100.f;
	alienPartSys.scale_vel = -0.2f;
}

void MainScene::EnterScene() 
{
	player.pos = vec(0.5f, 0.9f) * Camera::Size();
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
		new SimpleEnemy(vec(0.33f, 0.2f) * Camera::Size());
		auto orient_strategy = [this](StrategyEnemy& self, float dt) { 
			vec dir = player.pos - self.pos;
			// Add 90 deg, since it's facing south (90 deg) by default.
			self.rot_rads = -Angles::Pi/2.0f + std::atan2(dir.y, dir.x);
		};
		auto shoot_player_every_5sec_strategy = [this](StrategyEnemy& self, float dt, float total_time) {
			if (ShouldShootWithPeriod(0.5f, total_time, dt)) { new EnemyBullet(self.pos, player.pos - self.pos); }
		};
		new StrategyEnemy(vec(0.1f, 0.1f) * Camera::Size(), shoot_player_every_5sec_strategy, orient_strategy);
		break;
	}
}

void MainScene::ExitScene()
{
	alienPartSys.Clear();
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

	// Win
	if (timer <= 0) {
		timerText.SetString("Well done!");
		currentLevel++;
		Assets::winSnd.Play();
		Fx::FreezeImage::Freeze(2.0f);
		Fx::FreezeImage::SetUnfreezeCallback([]() {
			SceneManager::RestartScene();
			});
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

	for (SimpleEnemy* a : SimpleEnemy::GetAll()) {
		a->Update(dt);
	}
	for (StrategyEnemy* a : StrategyEnemy::GetAll()) {
		a->Update(dt);
	}

	for (Bullet* b : Bullet::GetAll()) {
		b->Update(dt);
		for (SimpleEnemy* a  : SimpleEnemy::GetAll()) {
			if (Collide(a,b)) {
				alienPartSys.pos = a->pos;
				alienPartSys.AddParticles(10);
				a->alive = false;
				b->alive = false;
			}
		}
	}

	for (EnemyBullet* b : EnemyBullet::GetAll()) {
		b->Update(dt);
		if (Collide(player.Bounds(), b->Bounds())) {
			Fx::FreezeImage::Freeze(0.5f);
			Assets::dieSnd.Play();
			Fx::FreezeImage::SetUnfreezeCallback([]() {
				SceneManager::RestartScene();
			});
		}
	}

	SimpleEnemy::DeleteNotAlive();
	Bullet::DeleteNotAlive();
	EnemyBullet::DeleteNotAlive();

	alienPartSys.UpdateParticles(dt);

}

void MainScene::Draw()
{
	Window::Clear(0, 0, 0);

	Window::Draw(Assets::backgroundTexture, Camera::Center())
		.withOrigin(Assets::backgroundTexture->w/2, Assets::backgroundTexture->h/2);

	if (Fx::FreezeImage::IsFrozen() && timer > 0.f) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
	}
	player.Draw();
	Shader::Deactivate();

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

	alienPartSys.Draw();

	Window::Draw(timerText, vec(Camera::Center().x, 30))
		.withOrigin(timerText.Size()/2)
		.withScale(0.666f);

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

	//alienPartSys.DrawImGUI();
#endif

}
